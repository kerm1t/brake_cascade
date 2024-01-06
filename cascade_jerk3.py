# -*- coding: utf-8 -*-
"""
Created on Wed Sep  6 22:34:56 2023

@author: uidk7714
"""
import matplotlib.pyplot as plt
import numpy as np
#import yaml

# Assumptions
t_react = 0.7
t_prefill = 0.225
t0 = t_react + t_prefill
# Bremsweg in Metern = (Anfangsgeschwindigkeit in m/s)^2 : doppelte Bremsverzögerung.
# ego acceleration
# not being used atm --> asdt = 0.5
# ego velocity
vmph = 75
vsdt = vmph*0.44704
# cascade [sec]
t_cascade = 2.0
t1 = t0+t_cascade

# jerk m/s^3
jerk = -2.5
jerk2 = -10
jerk3 = 5
# brake deceleration m/s^2
bsdt = -3.5
bsdt2 = -5
bsdt3 = 0

dstandoff = 3


# https://www.rechner.club/weg-zeit-geschwindigkeit/bremsweg-berechnen
state=0
a_t = []
a_d = []
a_v = []
a_b = []
a_j = []

#dt = 0.0001
dt = 0.0001   #  --> 10KHz
#dt = 0.000001 # --> 1 MHz = more accurate, but very slow
sim_freq = 1/dt # simulation frequency
sim_len = 11 # seconds

j = 0
a = 0
v = vsdt
d = 0

t_sim = 0
stage = 0
# t_stage = 0
t_fade = 0
t_stop = 0
for i in range(0, int(t0*sim_freq*sim_len), 1):
    t_sim = dt*i
    a = a + j*dt        
    v = v + a*dt
    d = d + v*dt
    # (1) system reaction (dead time)
    if t_sim<t0:
        j = 0
        a = 0
    # (2) 2 sec cascade 1
    elif t_sim<t1:
        if stage==0:
            stage = 1
            # t_stage = 0
        j = jerk
        if a<=bsdt:
            j = 0
    # (3) then cascade 2
    elif v > 2.5: # 1/2 * a (a = 5m/s^2) 
        j = jerk2
        if a<=bsdt2:
            j = 0
    # (4) v<5 slow decel
    else:
        if stage==1:
            stage = 2
            t_fade = t_sim # smoothe out deceleration
        j = jerk3
        if v<=0:
            a = 0
            j = 0
            if stage==2:
                stage=3
                t_stop = t_sim
                print("stopping at t=%.4f, v=%.4f" % (t_sim,v))
        if a>=0:
            j = 0
            if stage==2:
                stage=3
                t_stop = t_sim
#                print("stopping at t=%.4f, v=%.4f" % (t_sim,v))
    a_t.append(t_sim)
    a_d.append(d)
    a_v.append(v)
    a_b.append(a)
    a_j.append(j)
    

print ("stopping after [m]",d) # stopping after distance
print ("safety distance: ",d+dstandoff) # stopping after distance


        
col_ = ["#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b", "#e377c2", "#7f7f7f", "#bcbd22", "#17becf"]
    

fig = plt.figure()
# choose either:
grd=[411,412,413,414]
grd=[221,222,223,224]
#grd=[111,111,111,111]

bx = fig.add_subplot(grd[0])
bx.plot(a_t, a_j)
bx.set_xlabel("time [s]")   
bx.set_ylabel("jerk [m/s^3]") 
bx.grid()
plt.axline((t0,0), (t0,1), label="t-react",color=col_[0],linestyle=(0, (1, 0)),linewidth=1)
#plt.axline((t0+1.4,0), (t0+1.4,1), label="t-jerk",color=col_[1],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t0+t_cascade,0), (t0+t_cascade,1), label="t-cascade",color=col_[2],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t_fade,0), (t_fade,1), label="t-fade",color=col_[4],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t_stop,0), (t_stop,1), label="t-stop",color=col_[3],linestyle=(0, (1, 0)),linewidth=1)
# bx.set_title("Cascade %d [mph]" % vmph)
# plt.show()

bx = fig.add_subplot(grd[1])
bx.plot(a_t, a_b)
bx.set_xlabel("time [s]")   
bx.set_ylabel("a [m/s^2]") 
bx.grid()
plt.axline((t0,0), (t0,1), label="t-react",color=col_[0],linestyle=(0, (1, 0)),linewidth=1)
#plt.axline((t0+1.4,0), (t0+1.4,1), label="t-jerk",color=col_[1],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t0+t_cascade,0), (t0+t_cascade,1), label="t-cascade",color=col_[2],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t_fade,0), (t_fade,1), label="t-fade",color=col_[4],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t_stop,0), (t_stop,1), label="t-stop",color=col_[3],linestyle=(0, (1, 0)),linewidth=1)


#choose separate plots (yes/no)fig = plt.figure()
# bx = fig.add_subplot(111)
bx = fig.add_subplot(grd[2])

bx.plot(a_t, a_v)
bx.set_xlabel("time [s]")   
bx.set_ylabel("v [m/s]") 
bx.grid()
plt.axline((t0,0), (t0,1), label="t-react",color=col_[0],linestyle=(0, (1, 0)),linewidth=1)
#plt.axline((t0+1.4,0), (t0+1.4,1), label="t-jerk",color=col_[1],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t0+t_cascade,0), (t0+t_cascade,1), label="t-cascade",color=col_[2],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t_fade,0), (t_fade,1), label="t-fade",color=col_[4],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t_stop,0), (t_stop,1), label="t-stop",color=col_[3],linestyle=(0, (1, 0)),linewidth=1)

# start, end = bx.get_xlim()
# stepsize = 1.0
# bx.xaxis.set_ticks(np.arange(round(start), round(end), stepsize))
# start, end = bx.get_ylim()
# stepsize = 10
# bx.yaxis.set_ticks(np.arange(round(start/10)*10, round(end/10)*10, stepsize))



# fig = plt.figure()
# ax = fig.add_subplot(111)
ax = fig.add_subplot(grd[3])

ax.plot(a_t, a_d)
ax.set_xlabel("time [s]")   
ax.set_ylabel("d [m]") 
#ax.plot(t_stop, d_stop,"o",linewidth=2, markersize=4)
ax.grid()
plt.axline((t0,0), (t0,1), label="t-react",color=col_[0],linestyle=(0, (1, 0)),linewidth=1)
#plt.axline((t0+1.4,0), (t0+1.4,1), label="t-jerk",color=col_[1],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t0+t_cascade,0), (t0+t_cascade,1), label="t-cascade",color=col_[2],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t_fade,0), (t_fade,1), label="t-fade",color=col_[4],linestyle=(0, (1, 0)),linewidth=1)
plt.axline((t_stop,0), (t_stop,1), label="t-stop",color=col_[3],linestyle=(0, (1, 0)),linewidth=1)
ax.legend()


#ax.set_title("Cascade %d [mph]" % vmph)
fig.suptitle("Brake Cascade %d [mph], 1/dT=%d kHz" % (vmph,sim_freq/1000))
# start, end = ax.get_xlim()
# stepsize = 1.0
# ax.xaxis.set_ticks(np.arange(round(start), round(end), stepsize))
# start, end = ax.get_ylim()
# stepsize = 10
# ax.yaxis.set_ticks(np.arange(round(start/10)*10, round(end/10)*10, stepsize))
#fig.tight_layout(pad=.5)
plt.subplots_adjust(left=.0,
                    bottom=0.1, 
                    right=0.9, 
                    top=0.9, 
                    wspace=.25, 
                    hspace=.2)
