/*
A parametic lamp / object design by overflo for hackerspaceshop.com

This can be used with the usblinky usbstick to create a hypnotizing lavalamp / colorfading thing.

Built on openSCAD 2014.03




Links:
 http://www.hackerspaceshop.com/blinky/usblinky.html
 https://metalab.at




Thanks go to:
 Clifford Wolf (clifford.at) and Marius Kintel (kintel.net) for the FANTASTIC openSCAD software and their friendship.
 


Design revision: 2015.02.34

Released under CC-AT licence
Modify at will and cherish the love.
*/




// this should be set to 360 on final design, but during construction, rendering is MUCH faster if you keep this value low at around 50 or so ..
fn=50;



// the object is constructed of twisted spikes with a hollow core 


// how many spikes?
spikes = 100;

// how thick should a spike be?
thickness = 0.8;

// how twisted do we want them spikes in degree.?
// dont make a too steep twist or you will end up with hard to print overlaps on FDM-printers
twist = 90;

// how wide shall our construction be at maximum
outer_dia = 190;

// how height at maximum?
height = 190;


//dimensions of inner tube / spike

// inner spike diameter
spike_inner_dia = 15;

// how high?
spike_height = 180;










// IMPORTANT!
// modify this funtion to create a custom outline for your object
module add_outer_shell()
{

   w_factor = 0.5;
   h_factor=2.3;
   offset = 20;

   // make a long sphere
   translate([0, 0, (height/2)-20])  scale([w_factor,w_factor,h_factor]) sphere(50, $fn = fn, convexity=2);
}









// this creates a rod with a round end
// used in the middle of the object twice to make a hollow shaft
module spike(inset)
{

  h = spike_height - spike_inner_dia/2;
  cylinder(h = h, r = spike_inner_dia/2 - inset,$fn = fn);
  translate([0, 0, h]) sphere(spike_inner_dia/2 - inset, $fn = fn);
}




// create one fan element
module fan()
{

    linear_extrude(height = height, slices=250, twist = twist, $fn = fn, convexity=2 )      square([thickness, outer_dia/2]);

}



// rotate those fan elements

module fan_assembly()
{
  for (i = [0:spikes-1])
    rotate([0, 0, i*360/spikes]) fan();
}

























//.. the magic happens below this line
// i know this could be much nicer.. but its my first openSCAD script .. have mercy on me.
//////////////////////////////////////


difference()  // airflow opening on top to get hot air out.
{

intersection() // cut away overlap
{



intersection()  // add outer shell (make it round)
{


difference() // make hollow and remove cable outlet + 4 screw holes
{

union()  // base structure 
{


   // the rotated squares
   fan_assembly();

   // use this instead of the fan when tweaking with the outer shell
   //cylinder(r=outer_dia/2, h=height);
    

   // add a filled shaft in the middle with a round end on top
   translate([0,0,-5])  spike(thickness/2-1);

   // add the base (the round solid thing on the bottom taht we can screw onto)
   cylinder(r=spike_inner_dia/2+10,h=5, $fn = fn);




}


  // make the spike in the middle hollow
  translate([0,0,-6]) spike(thickness/2);

  // remove a little opening for the cables from the base
  // actually no. it looks much nicer with a lasercut acrylic base, but we keep this here for reference..s 
  // translate([0,0,-1]) rotate([90,0,45]) cylinder(r=5, h=100, $fn = fn);


  // remove 4 screwholes from the base
  translate([spike_inner_dia/2+5,0,0])cylinder(r=1.5, h=5, $fn = fn);
  translate([-(spike_inner_dia/2+5),0,0])cylinder(r=1.5, h=5,$fn = fn);
  translate([0,spike_inner_dia/2+5,0])cylinder(r=1.5, h=5,$fn = fn);
  translate([0,-(spike_inner_dia/2+5),0]) cylinder(r=1.5, h=5,$fn = fn);

}


 // now carve out the outer shell (the egg shape)
 add_outer_shell();


}







// cut away bottom overlaps

translate([0,0,height/2])  
cube([height,height,height],center=true);






}




// airflow opening on top, leds get hot, hot air needs to get out.
cylinder(h = height+100, r = 2, $fn=fn);


}