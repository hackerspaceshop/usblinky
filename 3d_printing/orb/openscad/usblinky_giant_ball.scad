


/*
A parametic lamp / lightobject design by overflo for hackerspaceshop.com / metalab.at

This can be used with the usblinky usbstick to create a hypnotizing lavalamp / colorfading thing.

Built on openSCAD 2014.03


Links:
 http://www.hackerspaceshop.com/blinky/usblinky.html
 https://metalab.at




Thanks go to:
Clifford Wolf, Marius Kintel and Philipp 'wizard23' Tiefenbacher for the FANTASTIC openSCAD software, their friendship and support.
 


Design revision: 2015.03.35

Released under CC-AT licence
Modify at will and cherish the love.
*/




/* for thingiverse / makerbot customizer */
// preview[view:south, tilt:side]


/* [Fan options] */


// Should we use fans or make a solid object?
use_fans =0;  //[1:Yes,0:No]




// How many blades should the fan be made of?
blades = 20; //[5:50]


// Direction of twist
left_right =2;  //[0:Left,1:Right,2:Both]



// How much shall we twist those blades? Dont make a too steep twist or you wont be``` able to print it. 30-90 looks good..
twist = 30;



// How thick should a blade be? 
thickness = 8;







/* [Outer shell options] */

// The width FACTOR of the outer shell is defined here. tweak it and see what happens..
//w_factor = 0.3;

//Move outer shall by this many mm down
offset = 10;



/* [General] */

inner_body_dia=190;


// How wide can it be (maximum rating, its probably going to be smaller than that)
outer_dia = 200;

// How tall should the object be
height = 200;



//Diameter of the collow shaft in the middle
spike_inner_dia = 15;





/* [Hidden] */


// how high?
spike_height = height -20;



fn=80;








// create one fan elementcccccccccccccccccccccccccccc
module fan()
{
	 // left twist
       if(left_right==0)   linear_extrude(height = height, slices=250, twist = -twist, $fn = fn, convexity=2 )      square([thickness, outer_dia/2]);
    // right twist (-twist)
       if(left_right==1)    linear_extrude(height = height, slices=250, twist = twist, $fn = fn, convexity=2 )      square([thickness, outer_dia/2]);

	// both directions
       if(left_right==2)
       {
linear_extrude(height = height, slices=250, twist = twist, $fn = fn, convexity=2 )      square([thickness, outer_dia/2]);
linear_extrude(height = height, slices=250, twist = -twist, $fn = fn, convexity=2 )  square([thickness, outer_dia/2]);   

       } 

}



// rotate those fan elements
module fan_assembly()
{
  for (i = [0:blades-1])
  {
   rotate([0, 0, i*360/blades]) fan();
  }
}



/////////////////////


module inner_body()
{
translate([0,0,inner_body_dia/2-5]) sphere(r=inner_body_dia/2, convexity=2,$fn=80);

}


module outer_body()
{
translate([0,0,outer_dia/2-4]) sphere(r=outer_dia/2,convexity=2,$fn=80);

}



////////////////////




// this creates a rod with a round end
// used in the middle of the object twice to make a hollow shaft
module spike(inset)
{

  h = spike_height - spike_inner_dia/2;
  cylinder(h = h, r = spike_inner_dia/2 - inset,$fn = fn);
  translate([0, 0, h]) sphere(spike_inner_dia/2 - inset, $fn = fn);
}


module mountbase()
{

difference()
{
  // add the base (the round solid thing on the bottom taht we can screw onto)
   cylinder(r=spike_inner_dia/2+10,h=5, $fn = fn);


  // remove 4 screwholes from the base
  translate([spike_inner_dia/2+5,0,0])cylinder(r=1.5, h=5, $fn = fn);
  translate([-(spike_inner_dia/2+5),0,0])cylinder(r=1.5, h=5,$fn = fn);
  translate([0,spike_inner_dia/2+5,0])cylinder(r=1.5, h=5,$fn = fn);
  translate([0,-(spike_inner_dia/2+5),0]) cylinder(r=1.5, h=5,$fn = fn);
}

}





difference()
{

intersection()
{

union()
{

// make faecher

 fan_assembly();


// make inner solid body

 inner_body();

//add spike (hollow core)

 spike(0);
mountbase();




}
// substract outer shape 


outer_body();


// cut away ueberstand
  translate([0,0,height/2])  
 cube([height,height,height],center=true);

}



 spike(1);
  // remove 4 screwholes from the base
  translate([spike_inner_dia/2+5,0,0])cylinder(r=1.5, h=5, $fn = fn);
  translate([-(spike_inner_dia/2+5),0,0])cylinder(r=1.5, h=5,$fn = fn);
  translate([0,spike_inner_dia/2+5,0])cylinder(r=1.5, h=5,$fn = fn);
  translate([0,-(spike_inner_dia/2+5),0]) cylinder(r=1.5, h=5,$fn = fn);
 
}

