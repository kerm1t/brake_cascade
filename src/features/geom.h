namespace lloft {
  class feature {
  };

  class point_feature : public feature {
    touchline;
    touchpoint;
  };

  class touchline : public point_feature {
  };
  
  class touchpoint : public point_feature {
  };

}