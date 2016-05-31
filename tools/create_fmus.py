import os
import sys

num_start=0 #numeration of one_robots-fmus
num_end=2  #

num_th=2 #number of fmus created in parallel
clean_up = 1 #clean up temporary files 0=off, 1=on
omc_set_flags="+simCodeTarget=Cpp +d=hpcom +n=2 +hpcomCode=openmp +hpcomScheduler=mcp" # set omc flags
fmu_target_folder="one_robot_fmus"


mo_output = ""
#need Roboter modeles, output in hpcom_tmp.mo
mo_header="package hpcom_tmp\nextends hpcom;\n"
mo_end="  annotation(Icon(coordinateSystem(extent = {{-100, -100}, {100, 100}}, preserveAspectRatio = true, initialScale = 0.1, grid = {2, 2})), Diagram(coordinateSystem(extent = {{-100, -100}, {100, 100}}, preserveAspectRatio = true, initialScale = 0.1, grid = {2, 2})));\nend hpcom_tmp;"

model_part1="  model fmu_one_robot"

model_part2="    Modelica.Blocks.Sources.Trapezoid trapezoidx(width = 1, amplitude = 0.2, rising = 1, falling = 1, period = 4, nperiod = -1, startTime = 1, offset = 1.5) annotation(Placement(transformation(extent = {{-186, 168}, {-166, 188}})));\n    Modelica.Blocks.Sources.Trapezoid trapezoidz(offset = 0, nperiod = -1, amplitude = 0.2, rising = 1, width = 1, falling = 1, period = 4, startTime = 1) annotation(Placement(transformation(extent = {{-186, 104}, {-166, 124}})));\n    Modelica.Blocks.Sources.Trapezoid trapezoidy(rising = 1, width = 2, falling = 1, period = 4, startTime = 1, nperiod = -1, amplitude = 0.3, offset = 1.5) annotation(Placement(transformation(extent = {{-186, 136}, {-166, 156}})));\n    hpcom.mechanical.Examples.robot_noEvent robot"
model_part3="(phi0 = -90) annotation(Placement(transformation(extent = {{94, 140}, {114, 160}})));\n    Modelica.Mechanics.MultiBody.Parts.PointMass pointMass(sphereDiameter = 0.2, m = 1) annotation(Placement(transformation(extent = {{2, 74}, {22, 94}})));\n    Modelica.Mechanics.MultiBody.Joints.Prismatic prismatic(useAxisFlange = true) annotation(Placement(transformation(extent = {{-100, 74}, {-80, 94}})));\n    Modelica.Mechanics.MultiBody.Joints.Prismatic prismatic1(useAxisFlange = true, n = {0, 0, 1}) annotation(Placement(transformation(extent = {{-42, 74}, {-22, 94}})));\n    Modelica.Mechanics.MultiBody.Joints.Prismatic prismatic2(useAxisFlange = true, n = {0, 1, 0}) annotation(Placement(transformation(extent = {{-72, 74}, {-52, 94}})));\n    Modelica.Mechanics.Translational.Sources.Position position annotation(Placement(transformation(extent = {{-106, 98}, {-86, 118}})));\n    Modelica.Mechanics.Translational.Sources.Position position1 annotation(Placement(transformation(extent = {{-84, 110}, {-64, 130}})));\n    Modelica.Mechanics.Translational.Sources.Position position2 annotation(Placement(transformation(extent = {{-52, 120}, {-32, 140}})));\n    inner Modelica.Mechanics.MultiBody.World world(enableAnimation = true) annotation(Placement(transformation(extent = {{-214, 74}, {-194, 94}})));\n  equation\n    connect(trapezoidx.y, robot1.xIn) annotation(Line(points = {{-165, 178}, {-134, 178}, {-134, 157.8}, {93.4, 157.8}}, color = {0, 0, 127}, smooth = Smooth.None));\n    connect(trapezoidy.y, robot1.yIn) annotation(Line(points = {{-165, 146}, {-134, 146}, {-134, 149.8}, {93.6, 149.8}}, color = {0, 0, 127}, smooth = Smooth.None));\n    connect(trapezoidz.y, robot1.zIn) annotation(Line(points = {{-165, 114}, {-136, 114}, {-136, 142}, {93.4, 142}}, color = {0, 0, 127}, smooth = Smooth.None));\n    connect(prismatic.frame_b, prismatic2.frame_a) annotation(Line(points = {{-80, 84}, {-72, 84}}, color = {95, 95, 95}, thickness = 0.5, smooth = Smooth.None));\n    connect(prismatic2.frame_b, prismatic1.frame_a) annotation(Line(points = {{-52, 84}, {-42, 84}}, color = {95, 95, 95}, thickness = 0.5, smooth = Smooth.None));\n    connect(prismatic1.frame_b, pointMass.frame_a) annotation(Line(points = {{-22, 84}, {12, 84}}, color = {95, 95, 95}, thickness = 0.5, smooth = Smooth.None));\n    connect(position.flange, prismatic.axis) annotation(Line(points = {{-86, 108}, {-84, 108}, {-84, 90}, {-82, 90}}, color = {0, 127, 0}, smooth = Smooth.None));\n    connect(position1.flange, prismatic2.axis) annotation(Line(points = {{-64, 120}, {-60, 120}, {-60, 90}, {-54, 90}}, color = {0, 127, 0}, smooth = Smooth.None));\n    connect(position2.flange, prismatic1.axis) annotation(Line(points = {{-32, 130}, {-28, 130}, {-28, 90}, {-24, 90}}, color = {0, 127, 0}, smooth = Smooth.None));\n    connect(position1.s_ref, robot1.yIn) annotation(Line(points = {{-86, 120}, {-88, 120}, {-88, 149.8}, {93.6, 149.8}}, color = {0, 0, 127}, smooth = Smooth.None));\n    connect(position2.s_ref, robot1.zIn) annotation(Line(points = {{-54, 130}, {-68, 130}, {-68, 142}, {93.4, 142}}, color = {0, 0, 127}, smooth = Smooth.None));\n    connect(world.frame_b, prismatic.frame_a) annotation(Line(points = {{-194, 84}, {-100, 84}}, color = {95, 95, 95}, thickness = 0.5, smooth = Smooth.None));\n    connect(position.s_ref, robot1.xIn) annotation(Line(points = {{-108, 108}, {-110, 108}, {-110, 158}, {93.4, 157.8}}, color = {0, 0, 127}, smooth = Smooth.None));\n    annotation(Diagram(coordinateSystem(preserveAspectRatio = false, extent = {{-220, -80}, {160, 200}}), graphics), Icon(coordinateSystem(extent = {{-220, -80}, {160, 200}})), experiment, __Dymola_experimentSetupOutput);\n  end fmu_one_robot"

mo_output=mo_header

for i in range(num_start,num_end):
    mo_output+=model_part1
    mo_output+=str(i)
    mo_output+="\n"
    mo_output+=model_part2
    mo_output+=str(1)
    mo_output+=model_part3
    mo_output+=str(i)
    mo_output+=";\n\n"
    
mo_output+=mo_end
hpcom_tmp_file = 'hpcom_tmp.mo'
mo_file = open(hpcom_tmp_file,'w')
mo_file.write(mo_output)
mo_file.close()
mo_output=[]

#need .mos files to create fmus (fmu_one_robot_tmp.mos) We create for every mo a .mos, because omc gets stack overflow during creating too many FMUs

mos_head="loadModel(Modelica, {\"3.2.1\"}); getErrorString();\nloadFile(\"../hpcom.mo\"); getErrorString();\nloadFile(\"../hpcom_tmp.mo\"); getErrorString();\nsetCommandLineOptions(\"" + omc_set_flags +"\"); getErrorString();\ntranslateModelFMU(hpcom_tmp.fmu_one_robot"
mos_bot=",stopTime= 1.0); getErrorString();\n"

for i in range(num_start,num_end):
    mos_output=mos_head
    mos_output+=str(i)
    mos_output+=mos_bot
    filename = "fmu_one_robot_tmp" + str(i) + ".mos"
    mos_file = open(filename,'w')
    mos_file.write(mos_output)
    mos_file.close()

helpfile_name = "cd1_eval2.sh"
helpfile_content = "#!/bin/bash\necho \"arg1:$1\"\necho \"arg2:$2\"\ncd $1\neval $2"
helpfile = open(helpfile_name,'w')
helpfile.write(helpfile_content)
helpfile.close()

#run .mos to generate fmus
for i in range(num_th):
	if not os.path.isdir("th"+str(i)):
		os.mkdir("th"+str(i))
for i in range((num_end-num_start)/num_th):
	exec_str=""
	for j in range(num_th):
		exec_str+="bash cd1_eval2.sh th"+str(j)+" 'omc ../fmu_one_robot_tmp"+str(i*num_th+j+num_start)+".mos'"
		if(j+1 != num_th):
			exec_str+=" | "
	print "cmd:"+exec_str
	if os.system(exec_str) != 0:
	  print "Error in creating fmu fmu_one_robot_tmp" +str(i*num_th+j)+".fmu and the following "+str(num_th)+" FMUs"
	  sys.exit(1)

#generate tmp folders for fmudata

if not os.path.isdir('tmp'):
	os.mkdir('tmp')
for i in range(num_start,num_end):
    if not os.path.isdir("tmp/tmp"+str(i)):
      os.mkdir("tmp/tmp"+str(i))

os.system('mkdir ' + fmu_target_folder)
os.system('mv th*/*.fmu ' + fmu_target_folder)


if clean_up:
	for i in range(num_th):
		os.system('rm -r th'+str(i))
	os.remove(hpcom_tmp_file)
	os.remove(helpfile_name)
	for i in range(num_start,num_end):
		filename = "fmu_one_robot_tmp" + str(i) + ".mos"
		os.remove(filename)
	
	
