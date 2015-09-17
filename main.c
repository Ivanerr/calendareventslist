#include <pebble.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define PLACE_NAME 2
#define EVENTS_DATA 3
  
#define string_sizes 66
#define EVENTS_LAYER_BUFFER_SIZE 1024

#ifdef PBL_COLOR  
#else
#define GColorRed GColorWhite
  #define GColorSunsetOrange GColorWhite
  #define GColorChromeYellow GColorWhite
  #define GColorGreen GColorWhite
  #define GColorVeryLightBlue GColorWhite
  #define GColorVividCerulean GColorWhite
  #define GColorPictonBlue GColorWhite
  #define GColorVividViolet GColorWhite
  #define GColorFolly GColorWhite
#endif
struct tm event_time1;
struct tm event_time2;
struct tm current_time_local;
struct tm *current_time;
static bool bt_connected=true;
static bool user_notified_of_loss_of_bt_connection=false;
static Window *s_main_window;
 
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
  
static TextLayer *s_time_layer;

static TextLayer *s_date_layer;
 
static TextLayer *first_event_time_layer;
static TextLayer *first_event_in_layer;
static TextLayer *first_event_description_layer;

static TextLayer *line_layer;
static TextLayer *line_layer1;
static TextLayer *line_layer2;

  
static TextLayer *second_event_time_layer;
static TextLayer *second_event_in_layer;
static TextLayer *second_event_description_layer;
static TextLayer *second_event_day_of_week_layer;

static TextLayer *third_event_time_layer;
static TextLayer *third_event_in_layer;
static TextLayer *third_event_description_layer;
static TextLayer *third_event_day_of_week_layer;



static GFont s_time_font;
static GFont the_smallest_font;
static GFont s_date_font;

static GFont event_time_font;
static GFont event_in_font;

static GFont event_description_font;

static GFont second_event_description_font;
static GFont gothic24;
static GFont second_event_time_font;
static GFont second_event_in_font;

char states[35][55];
char last_data_in_second_description[55];
char last_data_in_first_description[55];

static Layer *s_canvas_layer;

static void canvas_update_proc(Layer *this_layer, GContext *ctx) {
  /*GRect bounds = layer_get_bounds(this_layer);*/

  // Get the center of the screen (non full-screen)
  /*GPoint center = GPoint(bounds.size.w / 2, (bounds.size.h / 2));*/

  // Draw the 'loop' of the 'P'
/*  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 40);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 35);

  
  
  graphics_fill_rect(ctx, GRect(32, 40, 5, 100), 0, GCornerNone);*/
  graphics_context_set_stroke_color(ctx, GColorRed);
  graphics_draw_rect(ctx, GRect(0, 0, 144, 168));
  
  
//  graphics_context_set_fill_color(ctx, GColorRed);
}


static void load_storage_data()
  {
  int index;
     for(index=0;index<35;index++)
     {              
       persist_read_string(index,states[index],sizeof(states[index]));
       if(states[index][0]=='\0')
         break;
     }
//      APP_LOG(APP_LOG_LEVEL_INFO, "STORAGE LOADED");
  }

static void save_storage_data()
  {
  int index;
    for(index=0;index<35;index++)
     {
        //if(states[index][0]=='\0')
        //break;
        persist_delete(index);
        persist_write_string(index,states[index]);
     }
//  APP_LOG(APP_LOG_LEVEL_INFO, "STORAGE SAVED");
  }


void ftoa(char* str, double val, int precision) {
  //  start with positive/negative
  if (val < 0) {
    *(str++) = '-';
    val = -val;
  }
  //  integer value
  snprintf(str, 12, "%d", (int) val);
  str += strlen(str);
  val -= (int) val;
  //  decimals
  if ((precision > 0) && (val >= .00001)) {
    //  add period
    *(str++) = '.';
    //  loop through precision
    for (int i = 0;  i < precision;  i++)
      if (val > 0) {
        val *= 10;
        *(str++) = '0' + (int) (val + ((i == precision - 1) ? .5 : 0));
        val -= (int) val;
      } else
        break;
  }
  //  terminate
  *str = '\0';
}


static void differenceToWords(char *mystring,long int StartTime, long int EndTime,int screennumber)
{  
  
  static char tmpstring[36];
  long int difference;
  static char bufferDate1[50]="";
  static char bufferDate2[50]="";
  
  float minutes;
  time_t temp = time(NULL); 
  int real_minutes;
 // int eventhours1=0;
//  int eventhours2=0;
  
 /* for (unsigned int a=0;a<sizeof(bufferDate1);a++)
    {
       bufferDate1[a]='\0';
       bufferDate2[a]='\0';
    }*/

  switch(screennumber)
        {
         case 1: text_layer_set_text_color(first_event_in_layer, GColorWhite);
         case 0: text_layer_set_text_color(second_event_in_layer, GColorWhite);
         case 2: text_layer_set_text_color(third_event_in_layer, GColorWhite);
        }
  
 
    
  
  difference=StartTime-temp;
  
  time_t StartTime_time_t=StartTime;
  
  current_time=localtime(&temp);
  current_time_local=*current_time;
  
  current_time = localtime(&StartTime_time_t);
  event_time1 =*current_time;
  
  if(difference>99*60) //difference more than two hours
    {
       
        real_minutes=difference/60; //minutes
        minutes=difference/60/60; //thats hours
      
    
       ftoa(tmpstring, minutes, 4);
  
           
       
       /*time_t StartTime_time_t=StartTime;*/
       
       
          
    if(current_time_local.tm_hour-event_time1.tm_hour>0||minutes>24)
          switch(screennumber)
          {           
           
           case 0: strftime(bufferDate2,sizeof(bufferDate2),"%a",&event_time1); 
            text_layer_set_text(second_event_day_of_week_layer, bufferDate2); 
            break;
           case 2: strftime(bufferDate1,sizeof(bufferDate1),"%a",&event_time1); 
            text_layer_set_text(third_event_day_of_week_layer, bufferDate1); 
            break;
           case 1: break;
          }
    
    
       if((int)minutes>24)       
         {                                  
        
         /*if(StartTime==temp)
         snprintf(bufferDate,sizeof(bufferDate),"EQ");
         else
         snprintf(bufferDate,sizeof(bufferDate),"%d",(int)(StartTime-temp));*/
          //snprintf(bufferDate,sizeof(bufferDate),"%d",tick_time->tm_hour);                                    
           real_minutes=(int)minutes%24;
           minutes=minutes/24;
         
         snprintf(mystring, 19, "%dd %dhr", (int)minutes,(int)real_minutes);
           return;
         
        }
    
       real_minutes=real_minutes%60;
       snprintf(mystring, 19, "In %s:%02d", tmpstring,real_minutes);
       return;
    }
  else if(difference>60)        
    {    
    minutes=difference/60;
    ftoa(tmpstring, minutes, 7);
    
    if(minutes<30)
      {
      switch(screennumber)
        {
         case 1:text_layer_set_text_color(first_event_in_layer, GColorSunsetOrange);break;
         case 0:text_layer_set_text_color(second_event_in_layer, GColorSunsetOrange);break;
         case 2:text_layer_set_text_color(third_event_in_layer, GColorSunsetOrange);break;
        }
    }else
    if(minutes<60)
      {
      switch(screennumber)
        {
         case 1:text_layer_set_text_color(first_event_in_layer, GColorChromeYellow);break;
         case 0:text_layer_set_text_color(second_event_in_layer, GColorChromeYellow);break;
         case 2:text_layer_set_text_color(third_event_in_layer, GColorChromeYellow);break;
        }
     }
    
    
    snprintf(mystring, 19, "In %s min", tmpstring);
       return;
  }
  else if(difference<0)
    {
       difference = EndTime-temp;
      if(difference>60*60*2)
        {
            real_minutes=difference/60; //minutes
            minutes=difference/60/60; //hours
           ftoa(tmpstring, minutes, 4);
             switch(screennumber)
            {
               case 1:text_layer_set_text_color(first_event_in_layer, GColorGreen);break;
               case 0:text_layer_set_text_color(second_event_in_layer, GColorGreen);break;
               case 2:text_layer_set_text_color(third_event_in_layer, GColorGreen);break;
            }
                   
        real_minutes=real_minutes%60;
       snprintf(mystring, 19, "%s:%02d left", tmpstring,real_minutes);
        
        }
    else if(difference>60)        
      {
          minutes=difference/60;
         ftoa(tmpstring, minutes, 4);
           
          if(minutes<30)
            {
            switch(screennumber)
              {
               case 1:text_layer_set_text_color(first_event_in_layer,GColorVeryLightBlue );break;
               case 0:text_layer_set_text_color(second_event_in_layer, GColorVeryLightBlue);break;
               case 2:text_layer_set_text_color(third_event_in_layer, GColorVeryLightBlue);break;
              }
          }else
          if(minutes<=60)
            {
            switch(screennumber)
              {
               case 1:text_layer_set_text_color(first_event_in_layer,  GColorVividCerulean );break;
               case 0:text_layer_set_text_color(second_event_in_layer, GColorVividCerulean);break;
               case 2:text_layer_set_text_color(third_event_in_layer, GColorVividCerulean);break;
              }
           } else if(minutes<=120)
            {
            switch(screennumber)
              {
               case 1:text_layer_set_text_color(first_event_in_layer,GColorPictonBlue );break;
               case 0:text_layer_set_text_color(second_event_in_layer, GColorPictonBlue);break;
               case 2:text_layer_set_text_color(third_event_in_layer, GColorPictonBlue);break;              
              }
           }
      
        snprintf(mystring, 19,"%sm left", tmpstring);
      
      }
    else
      
      {
            minutes=difference;
           ftoa(tmpstring, minutes, 4);
             switch(screennumber)
            {
               case 1:text_layer_set_text_color(first_event_in_layer, GColorVividViolet);break;
               case 0:text_layer_set_text_color(second_event_in_layer, GColorVividViolet);break;
               case 2:text_layer_set_text_color(third_event_in_layer, GColorVividViolet);break;
            }
          snprintf(mystring, 19,"%ss left", tmpstring);
      }
         
    }
    else
    
    {      
      minutes=difference;
       ftoa(tmpstring, minutes, 4);
       snprintf(mystring, 19, "!!!");
        switch(screennumber)
        {
         case 1:text_layer_set_text_color(first_event_in_layer, GColorFolly);break;
         case 0:text_layer_set_text_color(second_event_in_layer, GColorFolly);break;
         case 2:text_layer_set_text_color(third_event_in_layer, GColorFolly);break;
        }
       return;
   }
  
  
  return;
}
static void DrawCalendar ()
  {
     
     static char first_event_location[string_sizes]="";
     static char first_event_time[string_sizes]="";
     static char first_event_time_end[string_sizes]="";
     
     static char first_event_complete_time[string_sizes]="";
     
     static char second_event_complete_time[string_sizes]="";
     static char third_event_complete_time[string_sizes]="";
     static char second_event_location[string_sizes]="";
     static char second_event_time[string_sizes]="";
     static char second_event_time_end[string_sizes]="";
  
     static char third_event_location[string_sizes]="";
     static char third_event_time[string_sizes]="";
     static char third_event_time_end[string_sizes]="";
   
   
  
  //text_layer_set_text(third_event_day_of_week_layer, "");
  //text_layer_set_text(second_event_day_of_week_layer, "");

  if(states[0][0]=='\0')
    {
    text_layer_set_text(first_event_description_layer, "!Please use the configuration");
   text_layer_set_text(second_event_description_layer, "button in your PT app");
   text_layer_set_text(first_event_in_layer, "");  
   text_layer_set_text(first_event_time_layer, "");
  
   //text_layer_set_text(second_event_description_layer, "");
   text_layer_set_text(second_event_in_layer, "");
   text_layer_set_text(second_event_day_of_week_layer, "");
  
   text_layer_set_text(second_event_time_layer, "");
  
   text_layer_set_text(third_event_description_layer, "");
   text_layer_set_text(third_event_in_layer, "");
   text_layer_set_text(third_event_time_layer, "");
   text_layer_set_text(third_event_day_of_week_layer, "");
   return;
   }
if(strcmp(states[0],"123454321")==0)
  {
   text_layer_set_text(first_event_description_layer, "No events");
   text_layer_set_text(first_event_in_layer, "");  
   text_layer_set_text(first_event_time_layer, "");
  
   text_layer_set_text(second_event_description_layer, "");
   text_layer_set_text(second_event_in_layer, "");
   text_layer_set_text(second_event_time_layer, "");
  
   text_layer_set_text(third_event_description_layer, "");
   text_layer_set_text(third_event_in_layer, "");
   text_layer_set_text(third_event_time_layer, "");
   text_layer_set_text(third_event_day_of_week_layer, "");
   return;
   }
  //APP_LOG(APP_LOG_LEVEL_INFO, "Got events string %s", events_layer_buffer);
  // APP_LOG(APP_LOG_LEVEL_INFO, "Length of recieved buffer %d", (int)strlen(events_layer_buffer));
    

  
  //find first two events that match our 
  bool foundfirstevent=false;
  bool foundsecondevent=false;
  bool foundthirdevent=false;
  time_t currenttime = time(NULL); 
  int i;
  for(i=0;i<35;i +=3)
      {
        if(atoi(states[i+2])>currenttime)
        {
           if(foundfirstevent==false)
            {
                strcpy(first_event_location, states[i]);
                strcpy(first_event_time, states[i+1]);
                strcpy(first_event_time_end, states[i+2]);                              
                foundfirstevent=true;
            }
          else if(foundsecondevent==false)
            {
              strcpy(second_event_location, states[i]);
              strcpy(second_event_time, states[i+1]);
              strcpy(second_event_time_end, states[i+2]);
              foundsecondevent=true;
            }
          else
            {
             foundthirdevent=true;
              strcpy(third_event_location, states[i]);
              strcpy(third_event_time, states[i+1]);
              strcpy(third_event_time_end, states[i+2]);
              break;
            }
        }
      }
  
  
  /*strcpy(first_event_location, states[0]);
  strcpy(first_event_time, states[1]);
  strcpy(first_event_time_end, states[2]);
  
  strcpy(second_event_location, states[3]);
  strcpy(second_event_time, states[4]);
  strcpy(second_event_time_end, states[5]);*/
  
  text_layer_set_text(second_event_day_of_week_layer, "");  
  text_layer_set_text(third_event_day_of_week_layer, "");  
  
  static char firstinstring[64]="";   
  static char secondinstring[64]="";  
  static char thirdinstring[64]="";  
  
  differenceToWords(firstinstring,atoi(first_event_time),atoi(first_event_time_end),1);
  

  
  if(foundsecondevent==true)
    {  
    layer_set_hidden(text_layer_get_layer(line_layer), false);
      differenceToWords(secondinstring,atoi(second_event_time),atoi(second_event_time_end),0);
    }
  else{
    layer_set_hidden(text_layer_get_layer(line_layer), true);
  }
  if(foundthirdevent==true)
    {  
  
    differenceToWords(thirdinstring,atoi(third_event_time),atoi(third_event_time_end),2);
  }
  
    
   
  
   text_layer_set_text(first_event_in_layer, firstinstring);  
   text_layer_set_text(first_event_time_layer, first_event_time);
  
   text_layer_set_text(second_event_description_layer, second_event_location);
   text_layer_set_text(second_event_in_layer, secondinstring);
   text_layer_set_text(second_event_time_layer, second_event_time);
  
   text_layer_set_text(third_event_description_layer, third_event_location);
   text_layer_set_text(third_event_in_layer, thirdinstring);
   text_layer_set_text(third_event_time_layer, third_event_time);
  
  
                       
  //first time
  
  time_t temp = atoi(first_event_time); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  //first timeend
  
   time_t temp_end = atoi(first_event_time_end); 
  struct tm *tick_time_end = localtime(&temp_end);


  static char buffer_end[] = "00:00";

  if(clock_is_24h_style() == true) {
  
    strftime(buffer_end, sizeof("00:00"), "%H:%M", tick_time_end);
  } else {
  
    strftime(buffer_end, sizeof("00:00"), "%I:%M", tick_time_end);
  }
  strcpy(first_event_complete_time,"");
  strcat(first_event_complete_time,buffer);
  strcat(first_event_complete_time,"-");
  strcat(first_event_complete_time,buffer_end);
  // Display this time on the TextLayer  
  text_layer_set_text(first_event_time_layer, first_event_complete_time);
  
  //FINISHED FIST ALL
  
  //second time
  time_t temp1 = atoi(second_event_time); 
  struct tm *tick_time1  = localtime(&temp1);
    static char buffer1[] = "00:00";
    if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer1, sizeof("00:00"), "%H:%M", tick_time1);
  } else {
    // Use 12 hour format
    strftime(buffer1, sizeof("00:00"), "%I:%M", tick_time1);
  }

  
  time_t temp1_end = atoi(second_event_time_end); 
  struct tm *tick_time1_end  = localtime(&temp1_end);
    static char buffer1_end[] = "00:00";
    if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer1_end, sizeof("00:00"), "%H:%M", tick_time1_end);
  } else {
    // Use 12 hour format
    strftime(buffer1_end, sizeof("00:00"), "%I:%M", tick_time1_end);
  }
  
  strcpy(second_event_complete_time,"");
  strcat(second_event_complete_time,buffer1);
  strcat(second_event_complete_time,"-");
  strcat(second_event_complete_time,buffer1_end);
  
  // Display this time on the TextLayer
  text_layer_set_text(second_event_time_layer, second_event_complete_time);
  
  //second time ends here
   // APP_LOG(APP_LOG_LEVEL_INFO, "Fe time:%s, Se time: %s", first_event_time,second_event_time);
  
  if(strcmp(states[3],"123454321")==0)
  {
   
   text_layer_set_text(second_event_description_layer, "");
   text_layer_set_text(second_event_in_layer, "");
   text_layer_set_text(second_event_time_layer, "");
   return;
   }
  
  //third time
  time_t temp2 = atoi(third_event_time); 
  struct tm *tick_time2  = localtime(&temp2);
    static char buffer2[] = "00:00";
    if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer2, sizeof("00:00"), "%H:%M", tick_time2);
  } else {
    // Use 12 hour format
    strftime(buffer2, sizeof("00:00"), "%I:%M", tick_time2);
  }

  
  time_t temp2_end = atoi(third_event_time_end); 
  struct tm *tick_time2_end  = localtime(&temp2_end);
    static char buffer2_end[] = "00:00";
    if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer2_end, sizeof("00:00"), "%H:%M", tick_time2_end);
  } else {
    // Use 12 hour format
    strftime(buffer2_end, sizeof("00:00"), "%I:%M", tick_time2_end);
  }
  
  strcpy(third_event_complete_time,"");
  strcat(third_event_complete_time,buffer2);
  strcat(third_event_complete_time,"-");
  strcat(third_event_complete_time,buffer2_end);
  
  // Display this time on the TextLayer
  text_layer_set_text(third_event_time_layer, third_event_complete_time);
  
  //second time ends here
   // APP_LOG(APP_LOG_LEVEL_INFO, "Fe time:%s, Se time: %s", first_event_time,second_event_time);
  
 /* if(strcmp(states[3],"123454321")==0)
  {
   
   text_layer_set_text(second_event_description_layer, "");
   text_layer_set_text(second_event_in_layer, "");
   text_layer_set_text(second_event_time_layer, "");
   return;
   }*/
  //second time end
  
  text_layer_set_text(first_event_description_layer, first_event_location);
  GSize first_layer_content_size=text_layer_get_content_size(first_event_description_layer);
  GSize second_layer_content_size=text_layer_get_content_size(second_event_description_layer);
  //text_layer_set_size(text_layer, (GSize){width, 20 });
  //first_event_description_layer=text_layer_create(GRect(0, 13, 144, 54));
  
  
  
  
  if(strcmp(last_data_in_second_description,second_event_location)!=0)
    {
      text_layer_set_font(second_event_description_layer,second_event_description_font);           
      strncpy(last_data_in_second_description,second_event_location,sizeof(last_data_in_second_description));          
     }
  if(strcmp(last_data_in_first_description,first_event_location)!=0)
    {
      text_layer_set_font(second_event_description_layer,second_event_description_font);              
      strncpy(last_data_in_first_description,first_event_location,sizeof(last_data_in_first_description));      
     }
  
//APP_LOG(APP_LOG_LEVEL_INFO, "first_layer_content_size.h=%d, second_layer_content_size.h: %d", first_layer_content_size.h,second_layer_content_size.h);
  if(first_layer_content_size.h>=48||second_layer_content_size.h>=36||foundthirdevent==false)
  { 
    //JUST TWO LINES
    layer_set_frame(text_layer_get_layer (line_layer), GRect(0,66,144,1)); //standart two-line height
    layer_set_frame(text_layer_get_layer (second_event_in_layer), GRect(0,62,144,18)); //standart two-line height
    layer_set_frame(text_layer_get_layer (second_event_time_layer), GRect(77,62,96,18)); //standart two-line height
    layer_set_frame(text_layer_get_layer (second_event_description_layer), GRect(0, 74, 144, 40)); //standart two-line heigh
    layer_set_frame(text_layer_get_layer (second_event_day_of_week_layer), GRect(44, 62, 32, 18)); //standart two-line heigh    
    layer_set_frame(text_layer_get_layer (line_layer1), GRect(0,114,144,1)); //standart two-line heightt    
    
        
                    layer_set_hidden(text_layer_get_layer(line_layer2), true);
                    layer_set_hidden(text_layer_get_layer(third_event_in_layer), true);
                    layer_set_hidden(text_layer_get_layer(third_event_time_layer), true);
                    layer_set_hidden(text_layer_get_layer(third_event_description_layer), true);
                    layer_set_hidden(text_layer_get_layer(third_event_day_of_week_layer), true);
    
    //JUST TWO LINES ENDS HERE
    
    
    if(first_layer_content_size.h==24) //if we have just one line of first layer move second layer up
      {
        layer_set_frame(text_layer_get_layer(line_layer),GRect(0,43,144,1)); //one-line height
        layer_set_frame(text_layer_get_layer (second_event_in_layer), GRect(0,39,144,18)); //standart one-line height
        layer_set_frame(text_layer_get_layer (second_event_time_layer), GRect(77,39,96,18)); //standart one-line height
        layer_set_frame(text_layer_get_layer (second_event_description_layer), GRect(0, 54, 144, 54)); //standart two-currenttime        
        layer_set_frame(text_layer_get_layer (second_event_day_of_week_layer), GRect(44, 39, 32, 18)); //standart two-line heigh    
        text_layer_set_font(second_event_description_layer,gothic24);        
      }    
    if(second_layer_content_size.w<120)
    {
      text_layer_set_font(second_event_description_layer,gothic24);    
      
      if(first_layer_content_size.h==24)
      layer_set_frame(text_layer_get_layer (second_event_description_layer), GRect(0, 54, 144, 60)); //standart two-line heigh
      else
      layer_set_frame(text_layer_get_layer (second_event_description_layer), GRect(0, 74, 144, 30)); //standart two-line heigh
    }
    
    //line_layer1=text_layer_create(GRect(0,114,144,1));
  }
  else
  { 
    //THREE LINES
    if(first_layer_content_size.h==24)
      {
        layer_set_frame(text_layer_get_layer(line_layer),GRect(0,43,144,1)); //one-line height
        layer_set_frame(text_layer_get_layer (second_event_in_layer), GRect(0,39,144,18)); //standart one-line height
        layer_set_frame(text_layer_get_layer (second_event_time_layer), GRect(77,39,96,18)); //standart one-line height
        layer_set_frame(text_layer_get_layer (second_event_description_layer), GRect(0, 54, 144, 36)); //standart two-currenttime        
        layer_set_frame(text_layer_get_layer (second_event_day_of_week_layer), GRect(44, 39, 32, 18)); //standart two-line heigh    
        if(second_layer_content_size.w>142)
          {
              //make third layer invisible  
                    layer_set_hidden(text_layer_get_layer(line_layer2), true);
                    layer_set_hidden(text_layer_get_layer(third_event_in_layer), true);
                    layer_set_hidden(text_layer_get_layer(third_event_time_layer), true);
                    layer_set_hidden(text_layer_get_layer(third_event_description_layer), true);
                    layer_set_hidden(text_layer_get_layer(third_event_day_of_week_layer), true);  
                    
          }
          else
          {
                    layer_set_frame(text_layer_get_layer (line_layer1), GRect(0,79,144,1)); //standart two-line height                                       
                    layer_set_hidden(text_layer_get_layer(line_layer2), false);
                    layer_set_hidden(text_layer_get_layer(third_event_in_layer), false);
                    layer_set_hidden(text_layer_get_layer(third_event_time_layer), false);
                    layer_set_hidden(text_layer_get_layer(third_event_description_layer), false);
                    layer_set_hidden(text_layer_get_layer(third_event_day_of_week_layer), false);
                    text_layer_set_font(second_event_description_layer,second_event_description_font);           
                 //make third layer visible   
          }
        
      }
  }
   
  if(foundsecondevent==false)
  {
    text_layer_set_text(second_event_description_layer, "");
    text_layer_set_text(second_event_in_layer, "");
    text_layer_set_text(second_event_time_layer, "");
    text_layer_set_text(second_event_day_of_week_layer, "");  
  }
  if(foundthirdevent==false)
    {
    
      text_layer_set_text(third_event_description_layer, "");
      text_layer_set_text(third_event_in_layer, "");
      text_layer_set_text(third_event_time_layer, "");
      text_layer_set_text(third_event_day_of_week_layer, "");  
    }
  //APP_LOG(APP_LOG_LEVEL_INFO, "FIRST Height: %d, Width: %d", first_layer_content_size.h,first_layer_content_size.w);
//  APP_LOG(APP_LOG_LEVEL_INFO, "Second Height: %d, Width: %d", second_layer_content_size.h,second_layer_content_size.w);
  
     return;
  }


static void main_window_load(Window *window) {
  
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Create Layer
  s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  

  //end of ramka
  
  window_set_background_color(window,GColorBlack);
  // Create GBitmap, then set to created 
  
s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_BLUETOOTH);
  
s_background_layer = bitmap_layer_create(GRect(124, 148, 20, 20));
bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_set_hidden(bitmap_layer_get_layer(s_background_layer),true);
//layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

                                                                      
  
  s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);
  
  
//create layers for EVENTS
  
  /*
  DRAW LINE
  static void point_layer_update_callback(Layer *layer, GContext *ctx) {
  GPoint p0 = GPoint(0, 0);
  GPoint p1 = GPoint(100, 100);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx, p0, p1);
}
d
*/
  
the_smallest_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  event_in_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

event_time_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
event_description_font = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);

gothic24=fonts_get_system_font(FONT_KEY_GOTHIC_24);
second_event_description_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
second_event_time_font = fonts_get_system_font(FONT_KEY_GOTHIC_18);
second_event_in_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);


first_event_in_layer=text_layer_create(GRect(0, -6, 144, 24));
first_event_time_layer=text_layer_create(GRect(77, 0, 96, 18));

  
first_event_description_layer=text_layer_create(GRect(0, 13, 144, 54));
text_layer_set_overflow_mode(first_event_description_layer,GTextOverflowModeFill);

line_layer=text_layer_create(GRect(0,67,144,1));
text_layer_set_background_color(line_layer, GColorWhite);
line_layer1=text_layer_create(GRect(0,114,144,1));
text_layer_set_background_color(line_layer1, GColorWhite);
line_layer2=text_layer_create(GRect(0,114,144,1));
text_layer_set_background_color(line_layer1, GColorWhite);

  //second event
second_event_in_layer=text_layer_create(GRect(0, 63, 144, 18));
second_event_time_layer=text_layer_create(GRect(77, 63, 96, 18));
second_event_description_layer=text_layer_create(GRect(0, 76, 144, 36));
second_event_day_of_week_layer=text_layer_create(GRect(44, 63, 32, 18));
text_layer_set_overflow_mode(second_event_description_layer,GTextOverflowModeFill);
  //second event ends
  //third event
  third_event_in_layer=text_layer_create(GRect(0, 75, 144, 18));
third_event_time_layer=text_layer_create(GRect(77, 75, 96, 18));
third_event_day_of_week_layer=text_layer_create(GRect(44, 75, 32, 18));
third_event_description_layer=text_layer_create(GRect(0, 90, 144, 22));
text_layer_set_overflow_mode(third_event_description_layer,GTextOverflowModeFill);
 text_layer_set_text_color(third_event_description_layer, GColorWhite);
text_layer_set_text_color(third_event_time_layer, GColorWhite);
text_layer_set_text_color(third_event_in_layer, GColorWhite);
 text_layer_set_text_color(third_event_day_of_week_layer, GColorWhite);
  text_layer_set_background_color(third_event_description_layer, GColorClear);
text_layer_set_background_color(third_event_time_layer, GColorBlack);
text_layer_set_background_color(third_event_in_layer, GColorClear);
text_layer_set_background_color(third_event_day_of_week_layer, GColorClear);
  text_layer_set_text_alignment(third_event_day_of_week_layer, GTextAlignmentRight);

  
text_layer_set_font(third_event_in_layer, second_event_in_font);
text_layer_set_font(third_event_time_layer, second_event_time_font);
text_layer_set_font(third_event_description_layer, second_event_description_font);
text_layer_set_font(third_event_day_of_week_layer, second_event_time_font);
  
  //third event ends
text_layer_set_text_color(first_event_time_layer, GColorWhite);
text_layer_set_text_color(first_event_in_layer, GColorWhite);
text_layer_set_text_color(first_event_description_layer, GColorWhite);  
text_layer_set_text_color(second_event_description_layer, GColorWhite);
text_layer_set_text_color(second_event_time_layer, GColorWhite);
text_layer_set_text_color(second_event_in_layer, GColorWhite);
  text_layer_set_text_color(second_event_day_of_week_layer, GColorWhite);
  


text_layer_set_background_color(first_event_description_layer, GColorClear);
text_layer_set_background_color(first_event_in_layer, GColorClear);
text_layer_set_background_color(first_event_time_layer, GColorClear);
  
text_layer_set_background_color(second_event_description_layer, GColorClear);
text_layer_set_background_color(second_event_time_layer, GColorBlack);
text_layer_set_background_color(second_event_in_layer, GColorClear);
  text_layer_set_background_color(second_event_day_of_week_layer, GColorClear);

  
text_layer_set_text_alignment(first_event_description_layer, GTextAlignmentLeft);
text_layer_set_text_alignment(second_event_description_layer, GTextAlignmentLeft);


text_layer_set_text_alignment(second_event_day_of_week_layer, GTextAlignmentRight);
text_layer_set_text_alignment(first_event_time_layer, GTextAlignmentLeft);
text_layer_set_text_alignment(first_event_in_layer, GTextAlignmentLeft);
text_layer_set_text_alignment(second_event_time_layer, GTextAlignmentLeft);
text_layer_set_text_alignment(second_event_in_layer, GTextAlignmentLeft);

  

text_layer_set_font(first_event_in_layer, event_in_font); 
text_layer_set_font(first_event_time_layer, event_time_font);
text_layer_set_font(first_event_description_layer, event_description_font);



text_layer_set_font(second_event_in_layer, second_event_in_font);
text_layer_set_font(second_event_time_layer, second_event_time_font);
text_layer_set_font(second_event_description_layer, second_event_description_font);
text_layer_set_font(second_event_day_of_week_layer, second_event_time_font);

  
text_layer_set_text(first_event_in_layer, "");
text_layer_set_text(second_event_in_layer, "");
text_layer_set_text(first_event_time_layer, "");
text_layer_set_text(second_event_time_layer, "");

text_layer_set_text(first_event_description_layer, "^Please use the configuration");
text_layer_set_text(second_event_description_layer, "button in your PT app");

  
text_layer_set_text(third_event_description_layer, "");
text_layer_set_text(third_event_in_layer, "");
text_layer_set_text(third_event_day_of_week_layer, "");
text_layer_set_text(third_event_time_layer, "");
  



layer_add_child(window_get_root_layer(window), text_layer_get_layer(third_event_day_of_week_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(third_event_in_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(third_event_time_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(third_event_description_layer));  
  

layer_add_child(window_get_root_layer(window), text_layer_get_layer(second_event_day_of_week_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(second_event_in_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(second_event_time_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(second_event_description_layer));  
  
layer_add_child(window_get_root_layer(window), text_layer_get_layer(first_event_description_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(first_event_time_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(first_event_in_layer));


  
//create layers for EVENTS END
  
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 105, 144, 42));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_overflow_mode(s_time_layer,GTextOverflowModeFill);
  
  
/*  text_layer_set_text(s_time_layer, "00:00");*/

  // Improve the layout to be more like a watchface
  //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, s_time_font);
  

  // Add it as a child layer to the Window's root layer
  

//s_date_layer=GRect(x, y, w, h);
s_date_layer = text_layer_create(GRect(0, 139, 144, 28));
s_date_font=  fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
text_layer_set_font(s_date_layer, s_date_font);
text_layer_set_background_color(s_date_layer, GColorBlack);
text_layer_set_text_color(s_date_layer, GColorWhite);
text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
text_layer_set_text(s_date_layer, "------");

  

// Create second custom font, apply it and add to Window
//s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_VERDANA_14));

layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

layer_add_child(window_get_root_layer(window), text_layer_get_layer(line_layer));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(line_layer1));
layer_add_child(window_get_root_layer(window), text_layer_get_layer(line_layer2));


  //layer_add_child(window_layer, s_canvas_layer);

  // Set the update_proc
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
load_storage_data();  
  if(states[0][0]!='\0')
    DrawCalendar();

}

static void main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
  // Destroy TextLayer
  // Destroy GBitmap
//gbitmap_destroy(s_background_bitmap);

// Destroy BitmapLayer
bitmap_layer_destroy(s_background_layer);
  
    //fonts_unload_custom_font(s_time_font);

text_layer_destroy(second_event_day_of_week_layer);
text_layer_destroy(third_event_day_of_week_layer);
text_layer_destroy(s_time_layer);


text_layer_destroy(s_date_layer);
//fonts_unload_custom_font(s_weather_font);
  
text_layer_destroy(first_event_time_layer);
text_layer_destroy(first_event_in_layer); 
text_layer_destroy(first_event_description_layer);
text_layer_destroy(second_event_time_layer);
text_layer_destroy(second_event_description_layer);
text_layer_destroy(second_event_in_layer);
text_layer_destroy(third_event_time_layer);
text_layer_destroy(third_event_description_layer);
text_layer_destroy(third_event_in_layer);
text_layer_destroy(line_layer);
text_layer_destroy(line_layer1);
text_layer_destroy(line_layer2);
  
//fonts_unload_custom_font(event_time_font);
//  fonts_unload_custom_font(event_in_font);
//fonts_unload_custom_font(event_description_font);

  save_storage_data();
}
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  
  
  static char bufferDate[96];

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  strftime(bufferDate,sizeof(bufferDate),"%a %d %b",tick_time);
  
  text_layer_set_text(s_date_layer, bufferDate);
  
  text_layer_set_text(s_time_layer, buffer);
  
  
  
}

static void SendRequestToFetchGoogleData()
  {
    // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
   
    bt_connected=bluetooth_connection_service_peek();
    if(bt_connected==false) //BT NOT connected
      {
          
      static uint32_t const segments[] = { 30, 100, 30, 100, 30, 100, 30, 100, 30, 100, 30,100, 30, 100, 30, 100, 30, 100, 30, 100, 30,  };
VibePattern pat = {
  .durations = segments,
  .num_segments = ARRAY_LENGTH(segments),
  };
      if(user_notified_of_loss_of_bt_connection==false) //WE HAVE NOT NOTIFIED THE USER OF THE LOSS OF BT CONNECTION
        {
           vibes_enqueue_custom_pattern(pat);
           user_notified_of_loss_of_bt_connection=true;          
        }
        layer_set_hidden(bitmap_layer_get_layer(s_background_layer),false);        
    }
    else //we have CONNECTED BT
      {
      if(user_notified_of_loss_of_bt_connection==true) //CONNECTION APPEARED
           {
                      SendRequestToFetchGoogleData();
                      layer_set_hidden(bitmap_layer_get_layer(s_background_layer),true);
           }
       user_notified_of_loss_of_bt_connection=false;
      }
  
if(tick_time->tm_sec == 0) { //REFRESH GOOGLE DATA ONCE A MINUTED
    if(bt_connected==true)
      {
         SendRequestToFetchGoogleData();
      }    
}
  if(tick_time->tm_sec % 30 == 0) { //we have to do that bit
    DrawCalendar(); 
  }  
  
update_time();
}





static void PopulateCalendar (char *events_layer_buffer)
  {
    int i=0;
    int g=0;
    int k=0;
  //clear all states
  for(k=0;k<35;k++)
  states[k][0]='\0';
  
  while(events_layer_buffer[i]!='\0')
    {
      if(events_layer_buffer[i]=='{'&&events_layer_buffer[i+1]=='!'&&events_layer_buffer[i+2]=='}')
        {
          i +=3;    
          
          for(k=0;k<4096;k++)
            {               
               if(events_layer_buffer[i]=='\0'||(events_layer_buffer[i]=='{'&&events_layer_buffer[i+1]=='!'&&events_layer_buffer[i+2]=='}'))
               {
                   //APP_LOG(APP_LOG_LEVEL_INFO, "Found EOS at %d",i);
                   //APP_LOG(APP_LOG_LEVEL_INFO, "i=%d Copying g=%d k=%d, char=EOS",i,g,k);                              
                   states[g][k]='\0';  //we found end of string
                 //  APP_LOG(APP_LOG_LEVEL_INFO, "String was !%s!",states[g]);
                   //APP_LOG(APP_LOG_LEVEL_INFO, "String was");
                   break;
               }                          
              
              // APP_LOG(APP_LOG_LEVEL_INFO, "i=%d Copying g=%d k=%d, char=%c",i,g,k,events_layer_buffer[i]);             
               states[g][k]=events_layer_buffer[i];             
               i++;
            }          
          g++;
        }
       else       
       {
         //APP_LOG(APP_LOG_LEVEL_ERROR, "Shouldnt be here %d",i);
         i++;
       }
    }
  }

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
    // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char place_name_buffer[32];
  static char weather_layer_buffer[64];
  static char events_layer_buffer[EVENTS_LAYER_BUFFER_SIZE];
  
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)t->value->int32);
      break;
    case KEY_CONDITIONS:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
      case PLACE_NAME:
      snprintf(place_name_buffer, sizeof(place_name_buffer), "%s", t->value->cstring);
      break;
      case EVENTS_DATA:
      snprintf(events_layer_buffer, sizeof(events_layer_buffer), "%s", t->value->cstring);
      PopulateCalendar(events_layer_buffer);DrawCalendar();
      break;
      
    default:
      //APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  //snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s\n%s", temperature_buffer, conditions_buffer);
  //text_layer_set_text(s_weather_layer, weather_layer_buffer);

}
static char reasonStr[20];

static void getAppMessageResult(AppMessageResult reason){
  switch(reason){
  case APP_MSG_CLOSED:
      snprintf(reasonStr,20,"%s","APP_MSG_CLOSED");
    break;
    case APP_MSG_INTERNAL_ERROR:
      snprintf(reasonStr,20,"%s","APP_MSG_INTERNAL_ERROR");
    break;
    case APP_MSG_OK:
      snprintf(reasonStr,20,"%s","APP_MSG_OK");
    break;
    case APP_MSG_SEND_TIMEOUT:
      snprintf(reasonStr,20,"%s","SEND TIMEOUT");
    break;
    case APP_MSG_SEND_REJECTED:
      snprintf(reasonStr,20,"%s","SEND REJECTED");
    break;
    case APP_MSG_NOT_CONNECTED:
      snprintf(reasonStr,20,"%s","NOT CONNECTED");
    break;
    case APP_MSG_APP_NOT_RUNNING:
      snprintf(reasonStr,20,"%s","NOT RUNNING");
    break;
    case APP_MSG_INVALID_ARGS:
      snprintf(reasonStr,20,"%s","INVALID ARGS");
    break;
    case APP_MSG_BUSY:
      snprintf(reasonStr,20,"%s","BUSY");
    break;
    case APP_MSG_BUFFER_OVERFLOW:
      snprintf(reasonStr,20,"%s","BUFFER OVERFLOW");
    break;
    case APP_MSG_ALREADY_RELEASED:
      snprintf(reasonStr,20,"%s","ALRDY RELEASED");
    break;
    case APP_MSG_CALLBACK_ALREADY_REGISTERED:
      snprintf(reasonStr,20,"%s","CLB ALR REG");
    break;
    case APP_MSG_CALLBACK_NOT_REGISTERED:
      snprintf(reasonStr,20,"%s","CLB NOT REG");
    break;
    case APP_MSG_OUT_OF_MEMORY:
      snprintf(reasonStr,20,"%s","OUT OF MEM");
    break;

  }

}


static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  
  
  //APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
  getAppMessageResult(reason);
  //APP_LOG(APP_LOG_LEVEL_ERROR, "%s", reasonStr);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
//  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  update_time();
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}



int main(void) {
  init();
  app_event_loop();
  deinit();
}
