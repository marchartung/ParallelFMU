model BouncingBall
  parameter Real e = 0.7;
  parameter Real g = 9.81;
  Real v;
  Boolean flying(start = true);
  Boolean impact;
  Real v_new;
  Modelica.Mechanics.MultiBody.Visualizers.Advanced.Shape shape(shapeType = "sphere", r_shape = {0, 0, 0}, length = 0.5, width = 0.5, height = 0.5, color = {150, 150, 0}, r = {0, h, 0}) annotation(Placement(transformation(extent = {{-20, 40}, {0, 60}})));
  Modelica.Blocks.Interfaces.RealOutput h(start = 1) annotation(Placement(visible = true, transformation(origin = {120, -2}, extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {-56, 0}, extent = {{-20, -20}, {20, 20}}, rotation = 0)));
equation
  impact = h <= 0.0;
  der(v) = if flying then -g else 0;
  der(h) = v;
  when {h <= 0.0 and v <= 0.0, impact} then
    v_new = if edge(impact) then -e * pre(v) else 0;
    flying = v_new > 0;
    reinit(v, v_new);
  end when;
  annotation(uses(Modelica(version = "3.2.1")));
end BouncingBall;