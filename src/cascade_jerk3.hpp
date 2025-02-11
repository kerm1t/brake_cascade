#ifndef CASCADE_JERK3_HPP
#define CASCADE_JERK3_HPP

#include <vector>
// implementation options
// (1) input t   --> any dt     <-- slide to any position within the demo --> (3) = can be a combination with (2)
//     - precalc
//     - calc t0 ... dt
// (2) increment --> fixed dt   <-- start
// 

namespace sim {
  
  class brake_cascade {
  public: // ugly, but we want to access velocity etc.
    // Assumptions
    float t_react = 0.7f;
    float t_prefill = 0.225f;
    float t0 = t_react + t_prefill;
    // Bremsweg in Metern = (Anfangsgeschwindigkeit in m / s) ^ 2 : doppelte Bremsverzögerung.
    // ego acceleration
    // not being used atm-- > asdt = 0.5
    // ego velocity
    float vmph = 75.f;
    float vsdt = vmph * 0.44704f;
    // cascade[sec]
    float t_cascade = 2.0f;
    float t1 = t0 + t_cascade;

    // jerk m / s ^ 3
    float jerk = -2.5f;
    float jerk2 = -10.f;
    float jerk3 = 5.f;
    // brake deceleration m / s ^ 2
    float bsdt = -3.5f;
    float bsdt2 = -5.f;
    float bsdt3 = 0.f;

    float dstandoff = 3.f;



    // (a) calc
    //float t; // or double ?
    float d = 0;
    float v = vsdt;
    float a = 0;
    float j = 0;

    // (b) store
    std::vector<float> a_t;
    std::vector<float> a_d;
    std::vector<float> a_v;
    std::vector<float> a_b; // == brake / acceleration
    std::vector<float> a_j;
    float dt = 0.005f;   //  -- > 10KHz

    int stage = 0;
    float t_fade = 0;
    float t_stop = 0;
  public:
    void init(float friction) {
      bsdt *= friction; // <-- is this physically correct?
      bsdt2 *= friction;
      bsdt3 *= friction;
    }
///    float brake(int i, float friction) {// can also put to constructor, as we don't change friction during sim
    float brake(int i) {

      //  https://www.rechner.club/weg-zeit-geschwindigkeit/bremsweg-berechnen
      float t_sim = dt * i;
      a = a + j * dt;
      v = v + a * dt;
      d = d + v * dt;

      // (1) system reaction (dead time)
      if (t_sim < t0) {
        j = 0;
        a = 0;
        // (2) 2 sec cascade 1
      }
      else if (t_sim < t1) {

        if (stage == 0) stage = 1;
        // t_stage = 0
        j = jerk;
        if (a <= bsdt) j = 0;
        // (3) then cascade 2
      }
      else if (v > 2.5) {
        // 1 / 2 * a(a = 5m / s ^ 2)
        j = jerk2;
        if (a <= bsdt2) j = 0;
        // (4) v<5 slow decel
      }
      else {
        if (stage == 1) {
          stage = 2;
          t_fade = t_sim; // smoothe out deceleration
        }
        j = jerk3;
        float v_stop = .5f;// 0.01f;  2do!! hmm, so we need to check all our physical scalar factors!! i.e. truck width height, brake distance, time etc.
        if (v <= v_stop) {
          j = 0;
          a = 0;
          v = 0; // new!
          if (stage == 2) {
            stage = 3;
            t_stop = t_sim;
            //          print("stopping at t=%.4f, v=%.4f" % (t_sim, v));
          }
        }
        if (a >= 0) {
          j = 0;
          if (stage == 2) {
            a = 0; // new!
            v = 0; // new!
            stage = 3;
            t_stop = t_sim;
            //                print("stopping at t=%.4f, v=%.4f" % (t_sim, v))

          }
        }
        if (stage < 3) { // sim ended
          a_t.push_back(t_sim);
          a_d.push_back(d);
          a_v.push_back(v);
          a_b.push_back(a);
          a_j.push_back(j);
        }

      }
      return d;
    } // brake()
  }; // class brake_cascade
} // namespace sim

#endif // CASCADE_JERK3_HPP
