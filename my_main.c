/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
                     multiply the current top of the
                     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
                    temporary matrix, multiply it by the
                    current top of the origins stack, then
                    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
        current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the screen
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"
#include "gmath.h"

void my_main() {

  int i;
  struct matrix *tmp;
  struct stack *systems;
  screen s;
  zbuffer zb;
  color c;
  double step_3d = 20;
  double theta;

  //Lighting values here for easy access
  color ambient;
  double light[2][3];
  double view[3];
  double areflect[3];
  double dreflect[3];
  double sreflect[3];

  ambient.red = 50;
  ambient.green = 50;
  ambient.blue = 50;

  light[LOCATION][0] = 0.5;
  light[LOCATION][1] = 0.75;
  light[LOCATION][2] = 1;

  light[COLOR][RED] = 0;
  light[COLOR][GREEN] = 255;
  light[COLOR][BLUE] = 255;

  view[0] = 0;
  view[1] = 0;
  view[2] = 1;

  areflect[RED] = 0.1;
  areflect[GREEN] = 0.1;
  areflect[BLUE] = 0.1;

  dreflect[RED] = 0.5;
  dreflect[GREEN] = 0.5;
  dreflect[BLUE] = 0.5;

  sreflect[RED] = 0.5;
  sreflect[GREEN] = 0.5;
  sreflect[BLUE] = 0.5;

  systems = new_stack();
  tmp = new_matrix(4, 1000);
  clear_screen( s );
  clear_zbuffer(zb);
  c.red = 0;
  c.green = 0;
  c.blue = 0;

  int q = 0;
  for(; q < lastop; q++){

    if(op[q].opcode == PUSH) push(systems);
    
    else if(op[q].opcode == POP) pop(systems);
    
    else if(op[q].opcode == BOX){
      add_box(tmp,
	      op[q].op.box.d0[0],
	      op[q].op.box.d0[1],
	      op[q].op.box.d0[2],
	      op[q].op.box.d1[0],
	      op[q].op.box.d1[1],
	      op[q].op.box.d1[2]);
      matrix_mult(peek(systems), tmp);
      draw_polygons(tmp, s, zb,
		    view, light, ambient, areflect, dreflect, sreflect);
      tmp->lastcol = 0;
    }
    
    else if(op[q].opcode == SPHERE){
      add_sphere(tmp,
		 op[q].op.sphere.d[0],
		 op[q].op.sphere.d[1],
		 op[q].op.sphere.d[2],
		 op[q].op.sphere.r,
		 step_3d);
      matrix_mult(peek(systems), tmp);
      draw_polygons(tmp, s, zb,
                    view, light, ambient, areflect, dreflect, sreflect);
      tmp->lastcol = 0;
    }

    else if(op[q].opcode == TORUS){
      add_torus(tmp,
		op[q].op.torus.d[0],
		op[q].op.torus.d[1],
		op[q].op.torus.d[2],
		op[q].op.torus.r0,
		op[q].op.torus.r1,
		step_3d);
      matrix_mult(peek(systems), tmp);
      draw_polygons(tmp, s, zb,
                    view, light, ambient, areflect, dreflect, sreflect);
      tmp->lastcol = 0;
    }

    else if(op[q].opcode == LINE){
      add_edge(tmp,
	      op[q].op.line.p0[0],
	      op[q].op.line.p0[1],
	      op[q].op.line.p0[2],
	      op[q].op.line.p1[0],
	      op[q].op.line.p1[1],
	      op[q].op.line.p1[2]);
      matrix_mult(peek(systems), tmp);
      draw_lines(tmp, s, zb, c);
      tmp->lastcol = 0;
    }

    else if(op[q].opcode == MOVE){
      tmp = make_translate(
			   op[q].op.move.d[0],
			   op[q].op.move.d[1],
			   op[q].op.move.d[2]);
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      tmp->lastcol = 0;
    }

    else if(op[q].opcode == SCALE){
      tmp = make_scale(
		       op[q].op.scale.d[0],
		       op[q].op.scale.d[1],
		       op[q].op.scale.d[2]);
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      tmp->lastcol = 0;
    }

    else if(op[q].opcode == ROTATE){
      if(op[q].op.rotate.axis == 0){
	tmp = make_rotX(op[q].op.rotate.degrees);
      }
      else if(op[q].op.rotate.axis == 1){
	tmp = make_rotY(op[q].op.rotate.degrees);
      }
      else if(op[q].op.rotate.axis == 2){
	tmp = make_rotZ(op[q].op.rotate.degrees);
      }
      matrix_mult(peek(systems), tmp);
      copy_matrix(tmp, peek(systems));
      tmp->lastcol = 0;
    }

    else if(op[q].opcode == DISPLAY){
      display(s);
    }
    
    else if(op[q].opcode == SAVE){
      save_extension(s, "tmp");
    }
  }
}
