model Blower
  parameter Real strength = 1.0;
  parameter Real time_fac = 1.0;
  output Real g;
equation
  g = strength * sin(time_fac*time);
end Blower;
