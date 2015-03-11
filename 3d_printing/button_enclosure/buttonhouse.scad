


module pcb()
{

union()
{
 difference()
 {
  union()
  {
   //pcb
   color("green") cube([22,32,1]);
   //button
   color("blue") translate([4,9,0]) cube(14);
  
   //cables
   color("red") translate([5,0,-2.5]) cube([12,8,6]);
   color("red") translate([5,24,-2.5]) cube([12,8,6]);

   // resistor
   color("yellow") translate([0,9,1]) cube([5,14,4]);


   // open below for solderspots
   color("red") translate([5,3.5,-2]) cube([12,8,2]);
   color("red") translate([5,20.5,-2]) cube([12,8,2]);

    color("red") translate([0,10,-2]) cube([4,12,2]);
    color("red") translate([18,10,-2]) cube([4,12,2]);

   color("red") translate([9,14,-2]) cube([4,4,2]);




   // open below for solderspots
  // color("red") translate([0,4,-2]) cube([22,24,2]);


   }
 
  //mounting holes
  translate([3.5,3.5,-1]) cylinder(r=1.6,h=10,$fn=20);
  translate([18.5,3.5,-1]) cylinder(r=1.6,h=10,$fn=20);
  translate([3.5,28.5,-1]) cylinder(r=1.6,h=10,$fn=20);
  translate([18.5,28.5,-1]) cylinder(r=1.6,h=10,$fn=20);
  }
 }
}

module cables()
{




  //cables out
  color("green") translate([7,31,-2]) cube([8,50,2]);
  color("green") translate([7,-50,-2]) cube([8,50,2]);





}




module screws()
{


 translate([3.5,3.5,-25]) cylinder(r=1.8,h=50,$fn=20);
 translate([3.5,3.5,5]) cylinder(r=3.5,h=20,$fn=20);


 translate([18.5,3.5,-25]) cylinder(r=1.8,h=50,$fn=20);
 translate([18.5,3.5,5]) cylinder(r=3.5,h=20,$fn=20);



 translate([3.5,28.5,-25]) cylinder(r=1.8,h=50,$fn=20);
 translate([3.5,28.5,5]) cylinder(r=3.5,h=20,$fn=20);




 translate([18.5,28.5,-25]) cylinder(r=1.8,h=50,$fn=20);
 translate([18.5,28.5,5]) cylinder(r=3.5,h=20,$fn=20);






}



module muttern(height=2)
{
 intersection()
 {
  cube([6.8,10,height], center=true);
  rotate([0,0,120]) cube([6.8,10,height], center=true);
  rotate([0,0,240]) cube([6.8,10,height], center=true);
 }

}



module hull()
{
$fn=10;
minkowski() {
  cube([26,36,12]);
   //rounded corners
  sphere(2, $fn=20);
}

}



module ueberstand()
{

intersection()
{
difference()
{



minkowski() {
  cube([26,36,10]);
   //rounded corners
  sphere(2, $fn=20);
}




translate([1,1,0]) minkowski() {
  cube([24,34,20]);
   //rounded corners
  sphere(2, $fn=20);
}




}

translate([-3,-3,3]) cube([100,100,2]);
}

}




module box()
{
difference()
{
 translate([-2,-2,-4.6]) hull();

 cables();
 screws();

 translate([3.5,3.5,-6]) muttern(5);
 translate([18.5,3.5,-6]) muttern(5);
 translate([3.5,28.5,-6]) muttern(5);
 translate([18.5,28.5,-6]) muttern(5);

 pcb();





}


}









rotate([0,180,0])
 difference()
{


 box(center=true);


translate([0,0,-50]) cube([100,100,100],center=true);
}


// ueberstand
translate([-24,-2,-3]) color("blue") ueberstand(); 




// pcb();



