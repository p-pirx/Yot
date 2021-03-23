// Yacht for pico display
// NC 3.3.21

#include "pico_display.hpp"
#include <math.h>
#include <time.h>
#include <cstdlib>

using namespace pimoroni;
extern unsigned char _binary_fastnet_tga_start[];

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);

int angle;
int angle_offset = 0;
int y_offset = 34;
int y;
int yotx = 50;
int yoty = 58;
int yot_knots = 1;
int lightx = 250;
int lighty = 0;
int light_width = 95;
int light_height = 110;
int light_knots = 1;
int sunx = 210;
int suny = 105;
int sleepy_time_ms = 2;  

uint8_t r,g,b ;

const uint16_t red         = pico_display.create_pen(255, 0, 0);
const uint16_t white_cloud = pico_display.create_pen(240,240,240);
const uint16_t white_sail  = pico_display.create_pen(230,230,230);
const uint16_t hull        = pico_display.create_pen(239,24,74);
const uint16_t bottom      = pico_display.create_pen(173,24,90);
const uint16_t varnish     = pico_display.create_pen(255,202,156);
const uint16_t black       = pico_display.create_pen(5, 5, 5);
const uint16_t sky_blue    = pico_display.create_pen(51, 153, 255);
const uint16_t sea_blue    = pico_display.create_pen(0, 51, 204);
const uint16_t sun         = pico_display.create_pen(255,117,0);

double degtorad(double degree) {  // degrees > radians
    double pi = 3.14159265359; 
    return (degree * (pi / 180)); 
}

void lighthouse(int xpos, int ypos){
    for(int y = 0; y < light_height; y++)
    {
       uint16_t *dest = pico_display.frame_buffer + (y * (light_width-1) * 3);    
       uint8_t *src = _binary_fastnet_tga_start + 18 + (y * light_width * 3);
      
       for(int x = 0; x < light_width; x++) {
          b = *src++;                          // Note bgr order
          g = *src++;
          r = *src++;
          pico_display.set_pen(r, g, b);       
          if(r < 253 ) {                       // Remove red background
              pico_display.pixel(Point(x + xpos, light_height - y + ypos));
          }
        }
    }
}

class cloud {
   public:
     int wait;
     int nump;
     int y;
     int x;
     int offset;
     int cycles = 0;
     int cparts[10][2];

     cloud(int n, int h, int v, int d, int f) {
        nump = n;
        x = h;
        y = v;
        wait = d;
        offset = f;
        upset();
     }

     void upset(){
         for (int i=0; i<nump; i++){
             cparts[i][1] = rand() % 10;
             cparts[i][2] = rand() % 8 + 5;                       
         } 
     }

     void show(){
        pico_display.set_pen(white_cloud);
        for (int i=0; i<nump; i++){
            pico_display.circle(Point(x+(i*offset), y+cparts[i][1]), cparts[i][2]); 
        }
        cycles++;        
        if (cycles > wait){
           x--;
           cycles = 0;
        }
        if (x < -100){
            x = 245;
            cycles = 0;
            upset();
        }
    }
};

void yot(int x, int y) {
    y = y+5;
    pico_display.set_pen(white_sail);
    pico_display.triangle(Point(x+2,y+1), Point(x+19,y-4), Point(x+2, y+40));   // jib
    pico_display.triangle(Point(x-20,y), Point(x+1,y), Point(x+1, y+55));       // main

    pico_display.set_pen(hull);            // topsides
    Rect topside(x-24, y-10, 39, 6);
    pico_display.rectangle(topside);
    pico_display.triangle(Point(x-24,y-10), Point(x-24,y-4), Point(x-28, y-4));  // counter
    pico_display.triangle(Point(x+20,y-4), Point(x+14,y-4), Point(x+14, y-10));  // bow

    pico_display.set_pen(bottom);          // antifouling
    Rect keel(x-24, y-15, 33, 5);
    pico_display.rectangle(keel);
    pico_display.triangle(Point(x+14,y-10), Point(x+9,y-10), Point(x+8, y-15));   // forefoot

    pico_display.set_pen(varnish);          // wood
    Rect mast(x, y-4, 2,56);
    pico_display.rectangle(mast);           // mast
    Rect boom(x-20, y, 20,2);
    pico_display.rectangle(boom);           // boom
    Rect doghouse(x-16, y-4, 10,3);
    pico_display.rectangle(doghouse);       // doghouse
    Rect staff(x-27, y-4, 1, 5);
    pico_display.rectangle(staff);          // ensign staff

    pico_display.set_pen(black);
    Rect window(x-14, y-3, 6, 1);
    pico_display.rectangle(window);         // window
 
    pico_display.set_pen(red);
    pico_display.triangle(Point(x+2,y+51), Point(x+2,y+56), Point(x-6, y+53));  // burgee   
    Rect crew(x-22, y-4, 2, 3);
    pico_display.rectangle(crew);           // crew
    Rect ensign(x-29, y-1, 3, 2);
    pico_display.rectangle(ensign);         // ensign
}

int main() {
    pico_display.init();
    pico_display.set_backlight(150);    

    cloud cloud1(9,150,90,10,12); // no blobs, init x, init y, delay, x offset
    cloud cloud2(7,50,100,8,11); 

    while(true) {

        pico_display.set_pen(sky_blue);        //   blue sky
        pico_display.clear();
        
        pico_display.set_pen(sun);             //   sun
        pico_display.circle(Point(sunx, suny), 15);
  
        cloud1.show();
        cloud2.show();

        lighthouse(lightx, lighty);

        yot(yotx,yoty);      

        for(int x=0; x<240; x+=1) {
            angle = (x*1.5)+angle_offset;
            y = int(sin(degtorad(angle)) * 15) + y_offset;
  
            pico_display.set_pen(sea_blue);                   //dark blue wave
            pico_display.line(Point(x, 0), Point(x, y));
          
            if(x==yotx || yotx > 239){
               yoty = y+6;
            }                  
        }  
       
        pico_display.update();
    
        sleep_ms(sleepy_time_ms);  

        angle_offset = angle_offset+10;
        if(angle_offset>360) {
            angle_offset = 0;

            yotx = yotx + yot_knots;
            if (yotx > 270) {
                 yotx = 0;
            }

            lightx = lightx - light_knots;
            if(lightx < -65){
                lightx = 270;
            }
                
        }
    } 
}


