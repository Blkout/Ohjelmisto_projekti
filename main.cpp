#include <Box2D/Box2D.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cstdio>
#include "myContactListener.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <Box2D\Dynamics\Joints\b2MouseJoint.h>

//TODO: ylilentäneiden poistaminen, stack overflowt anglessa yms jos tulee 100 laatikkoa. yms. pientä laittoa. :D


// DEFINE SIZE OF SCREEN
const int SCREEN_W = 1024;
const int SCREEN_H = 768;

// The scale: 32 pixels is 1meter
const int PIX_METER = 32;

// Set up refresh rate here
const int FPS = 60;

// How many small boxes
int howManyBodies = 0;


// DEFINE KEYS
bool key[5] = {false,false,false,false,false};
enum MYKEYS 
{
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_W
};

// MAIN PROGRAM

int main(int argc, char** argv)
{
    // GLOBAL VARIABLES
    float mouse_x = 0;
    float mouse_y = 0;
    int body_x = 0;
    int body_y = 0;
    int timeFactor = 1;
	int itemSpawning = 0;
	char spawnTypes[3][10] = {"Crates","Circles","Triangles"};
    bool itemClicked = false;
	bool doExit = false;
	int camera_x = 0;
	int camera_y = 0;
	float backpos_x = 0;
	float backpos_y = 0;
	b2Vec2 velocity;
	int first_screen_pos = 0;

	// level = 0 -> First screen ( options for singeplayer, multiplayer, options?, credits? etc not used atm.)

	int level = 0;

	// If the game is paused, box2d wont calculate anything and will be paused too.

	bool gamePaused = false;
	

    B2_NOT_USED(argc);
    B2_NOT_USED(argv);

    // SETUP GRAVITY
    b2Vec2 gravity(0.0f, 10.0f);
    bool doSleep = true;
    b2World world(gravity);

    // DEFINE GROUND BODY
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(16,23);
    b2Body* groundBody = world.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    float g_width = 32.0f;
    float g_height = 2.0f;
    groundBox.SetAsBox(g_width/2, g_height/2);

    // GROUND FIXTURE
    groundBody->CreateFixture(&groundBox, 0.0f);
    groundBody->SetUserData("ground");

	b2BodyDef groundBodyDef2;
    groundBodyDef2.position.Set(0,16);
    b2Body* groundBody2 = world.CreateBody(&groundBodyDef2);
    b2PolygonShape groundBox2;
    float g2_width = 2.0f;
    float g2_height = 32.0f;
    groundBox.SetAsBox(g2_width/2, g2_height/2);

    // GROUND FIXTURE
    groundBody2->CreateFixture(&groundBox, 0.0f);
    groundBody2->SetUserData("ground");

    // DEFINE PLAYER
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set((float)(SCREEN_W/2)/PIX_METER, 20.0f); //top of the screen
    b2Body* body = world.CreateBody(&bodyDef);
    body->SetUserData("box");
 
    // DEFINE BOX
    b2BodyDef bodyDef2;
    bodyDef2.type = b2_dynamicBody;
    bodyDef2.position.Set((float)(SCREEN_W/2)/PIX_METER, 10.0f); //top of the screen
    b2Body* body2 = world.CreateBody(&bodyDef2);
    body2->SetUserData("box2");

    // Storage for small boxes
    b2Body* bodies[100] = {};
    float position_x[100] = {};
    float position_y[100] = {};

    // Define another box shape for our dynamic body.
    b2PolygonShape dynamicBox;
    float d_width = 2.0f;
    float d_height = 2.0f;
    dynamicBox.SetAsBox(d_width/2, d_height/2);


    // DEFINE FIXTURE FOR PLAYER
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &dynamicBox;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.9f;
    body->CreateFixture(&fixtureDef);
    body2->CreateFixture(&fixtureDef);

    b2PolygonShape dynamicBox2;
    float x_width = 1.0f;
    float x_height = 1.0f;
    dynamicBox2.SetAsBox(x_width/2, x_height/2);

	// ATTEMPT AT CIRCLE SHAPED OBJECTS

	b2BodyDef ballBodyDef;
	ballBodyDef.type = b2_dynamicBody;
	ballBodyDef.position.Set(10.0f,6.0f);
	b2Body* body5 = world.CreateBody(&ballBodyDef);
	b2CircleShape circle;
	circle.m_radius = 0.5f; 
	b2FixtureDef ballShapeDef;
	ballShapeDef.shape = &circle;
	ballShapeDef.density = 1.0f;
	ballShapeDef.friction = 0.2f;
	ballShapeDef.restitution = 0.8f;
	body5->CreateFixture(&ballShapeDef);



	// DEFINE FIXTURES FOR SMALL BOXES

    b2FixtureDef fixtureDef3;
    fixtureDef3.shape = &dynamicBox2;
    fixtureDef3.density = 1.0f;
    fixtureDef3.friction = 0.9f;
	fixtureDef3.restitution = 0.4f;
	
	b2BodyDef bodyDef3;
	bodyDef3.type = b2_dynamicBody;
	for(int a=1; a<10; a++)
	{
		bodyDef3.position.Set(5,22-a*1);
		//body2->SetUserData("box2");
		bodies[howManyBodies] = world.CreateBody(&bodyDef3);
		bodies[howManyBodies]->CreateFixture(&fixtureDef3);
		howManyBodies++;
	}

    // give the player some spin and some motion to make it interesting
    body->SetAngularVelocity(0.8f);
    b2Vec2 initial_v (1.6f, 0.0f);
    body->SetLinearVelocity(initial_v);
    world.SetContactListener(new my_listener());



/*
  				  b2MouseJointDef mousedef;
				  mousedef.bodyA = groundBody;
				  mousedef.bodyB = body2;
				  b2Vec2 bodyPos = body2->GetPosition();
				  //mousedef.target.Set(bodyPos.x,bodyPos.y);			NOT WORKING MOUSE-JOINT TRY TO FIX LATER
				  mousedef.collideConnected = true;
				  mousedef.maxForce = 100 * body2->GetMass();
				  body2->SetAwake(true);
				  //b2Joint* joint = world.CreateJoint(&mousedef);
*/



    // Prepare for simulation. Typically we use a time step of 1/60 of a
    // second (60Hz) and 10 iterations. This provides a high quality simulation
    // in most game scenarios.

    float32 timeStep = 1.0f / FPS;
    int32 velocityIterations = 10;
    int32 positionIterations = 10;

    // PREPARE FOR DRAWING
    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_BITMAP *first_screen = NULL;
	ALLEGRO_BITMAP *ball = NULL;
    ALLEGRO_BITMAP *ground = NULL;
	ALLEGRO_BITMAP *smallbox = NULL;
	ALLEGRO_BITMAP *player = NULL;
	ALLEGRO_BITMAP *background = NULL;
	ALLEGRO_BITMAP *platform = NULL;
    bool redraw = true;


	// INITIALIZE ALLEGRO
    if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
    }

	// INITIALIZE ALLEGRO IMAGE ADDON
	if(!al_init_image_addon())
	{
		fprintf(stderr, "failed to initialize the image addon!\n");
        return -1;
	}

	// INSTALL KEYBOARD
	if(!al_install_keyboard())
	{
        fprintf(stderr, "failed to initialize the keyboard!\n");
        return -1;
    }

	// INSTALL MOUSE
	if(!al_install_mouse())
    {
        fprintf(stderr, "failed to initialize the mouse!\n");
        return -1;
    }

	// CREATE TIMER
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
      fprintf(stderr, "failed to create timer!\n");
      return -1;
    }

	// CREATE DISPLAY
	al_set_new_display_flags(ALLEGRO_FULLSCREEN);			// FULLSCREEN MODE IF NEEDED!
	al_set_new_window_position(0,0);
    display = al_create_display(SCREEN_W, SCREEN_H);
    if(!display) {
      fprintf(stderr, "failed to create display!\n");
      al_destroy_timer(timer);
      return -1;
    }

	// CREATING GROUND LAYER
    ground = al_create_bitmap(g_width*PIX_METER, g_height*PIX_METER);
    if(!ground) {
      fprintf(stderr, "failed to create ground bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }

		// LOADING IMAGE FOR FIRST SCREEN
	first_screen = al_load_bitmap("aloitusruutu.png");
    if(!first_screen) {
      fprintf(stderr, "failed to create first screen bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
	}

	// LOADING IMAGE FOR SMALL BOXES
	smallbox = al_load_bitmap("crate.png");
    if(!smallbox) {
      fprintf(stderr, "failed to create small box bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
	}

	// LOADING IMAGE FOR BALL
	ball = al_load_bitmap("ball.png");
    if(!ball) {
      fprintf(stderr, "failed to create ball bitmap!\n");
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
	}

	// LOADING BACKGROUND IMAGE
		background = al_load_bitmap("taivas.png");
    if(!background) {
      fprintf(stderr, "failed to create box bitmap!\n");
      al_destroy_bitmap(ground);
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -5;
    }

	// LOADING PLATFORM IMAGE
		platform = al_load_bitmap("maa.png");
    if(!platform) {
      fprintf(stderr, "failed to create box bitmap!\n");
      al_destroy_bitmap(ground);
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -5;
    }

	// LOADING PLAYER IMAGE
	player = al_load_bitmap("box.png");
    if(!player) {
      fprintf(stderr, "failed to create box bitmap!\n");
      al_destroy_bitmap(ground);
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -5;
    }

	// LOAD FONT ADDON FOR COOLER FONTS

	al_init_font_addon(); // initialize the font addon
    al_init_ttf_addon();// initialize the ttf (True Type Font) addon
    ALLEGRO_FONT *font = al_load_ttf_font("pirulen.ttf",28,0 );

	// MASKING
	al_set_target_bitmap(ball);
	al_convert_mask_to_alpha(ball, al_map_rgb(255,0,255));
	al_set_target_bitmap(player);
	al_convert_mask_to_alpha(player, al_map_rgb(255,0,255));
    al_set_target_bitmap(ground);
    al_clear_to_color(al_map_rgb(255, 0, 255));
    al_set_target_bitmap(al_get_backbuffer(display));

	// CREATE EVENT QUEUE
    event_queue = al_create_event_queue();
    if(!event_queue) {
      fprintf(stderr, "failed to create event_queue!\n");
      al_destroy_bitmap(ground);
      al_destroy_display(display);
      al_destroy_timer(timer);
      return -1;
    }
	
	// REGISTERING EVENTS AND STARTING THE GAME
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();
    al_start_timer(timer);

    // GAMELOOP
    while (!doExit)
    {
		ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

	    if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
	    {
			float32 x = ev.mouse.x/32;
			float32 y = ev.mouse.y/32;
			b2Vec2 pos;
			pos.x = x;
			pos.y = y;

			if(itemSpawning == 0)
			{
				b2BodyDef bodyDef3;
				bodyDef3.type = b2_dynamicBody;
				bodyDef3.position.Set(x,y);
				body2->SetUserData("box2");
				bodies[howManyBodies] = world.CreateBody(&bodyDef3);
				bodies[howManyBodies]->CreateFixture(&fixtureDef3);
				howManyBodies++;
			}
			if(itemSpawning == 1)
			{

			}
			    //mousedef.target.Set(x,y);
				//b2Joint* joint = world.CreateJoint(&mousedef);		// FAILED ATTEMPT AT MOUSEJOINTS

		    mouse_x = ev.mouse.x;
		    mouse_y = ev.mouse.y;
		    mouse_x = mouse_x/32;
		    mouse_y = mouse_y/32;


			// Fun effect ^^
		    /*
		    for(int w = 0; w<howManyBodies-1; w++)
		    {
			    bodies[w]->ApplyForce(b2Vec2((bodies[w]->GetPosition().x - mouse_x)*-300,(bodies[w]->GetPosition().y-mouse_y)*-300), bodies[w]->GetWorldCenter());
		    }
		    */
	    }

		// Is called when mouse button is released
	    if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
	    {

	    }
	    
		// Is called whenever mouse moves on screen
	    if(ev.type == ALLEGRO_EVENT_MOUSE_AXES ||
		ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) 
		{
			  mouse_x = ev.mouse.x;
			  mouse_y = ev.mouse.y;
			  mouse_x = mouse_x/32;
			  mouse_y = mouse_y/32;
			  //body->ApplyForce(b2Vec2((body->GetPosition().x - mouse_x)*-5,(body->GetPosition().y-mouse_y)*-5), body->GetWorldCenter());  // FOLLOW MOUSE EFFECT
        }

		// Is called on timestep (1/60)
        if(ev.type == ALLEGRO_EVENT_TIMER) 
		{
			camera_x = (body->GetPosition().x/PIX_METER);
			camera_y = (body->GetPosition().y/PIX_METER);

			velocity = body->GetLinearVelocity();

			if(key[KEY_UP]) 
			{
				 body->ApplyForce(b2Vec2(0,-500), body->GetWorldCenter());
			}

			if(key[KEY_DOWN]) 
			{
				body->ApplyForce(b2Vec2(0,500), body->GetWorldCenter());
			}

			if(key[KEY_LEFT]) 
			{
				//body->SetAngularVelocity(body->GetAngularVelocity()-3);
				body->ApplyForce(b2Vec2(-500,0), body->GetWorldCenter());
			}

			if(key[KEY_RIGHT]) 
			{
				//body->SetAngularVelocity(body->GetAngularVelocity()+3);
				body->ApplyForce(b2Vec2(500,0), body->GetWorldCenter());
			}

			if(key[KEY_W])
			{
				//itemSpawning++;
			}

            world.Step(timeStep, velocityIterations, positionIterations);
            world.ClearForces();
            redraw = true;
        }
        else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
		{
            break;
        }

		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) 
		{
         switch(ev.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
               key[KEY_UP] = true;
               break;
 
            case ALLEGRO_KEY_DOWN:
               key[KEY_DOWN] = true;
               break;
 
            case ALLEGRO_KEY_LEFT: 
               key[KEY_LEFT] = true;
               break;
 
            case ALLEGRO_KEY_RIGHT:
               key[KEY_RIGHT] = true;
               break;

			case ALLEGRO_KEY_W:
               key[KEY_W] = true;
			   itemSpawning++;
               break;
			}
		}

		else if(ev.type == ALLEGRO_EVENT_KEY_UP)
		{
			switch(ev.keyboard.keycode) {
            case ALLEGRO_KEY_UP:
               key[KEY_UP] = false;
               break;
 
            case ALLEGRO_KEY_DOWN:
               key[KEY_DOWN] = false;
               break;
 
            case ALLEGRO_KEY_LEFT: 
               key[KEY_LEFT] = false;
               break;
 
            case ALLEGRO_KEY_RIGHT:
               key[KEY_RIGHT] = false;
               break;

			case ALLEGRO_KEY_W:
               key[KEY_W] = false;
			   if(itemSpawning == 3)
			   {
			   itemSpawning = 0;
			   }
               break;
 
            case ALLEGRO_KEY_ESCAPE:
               doExit = true;
               break;
			}	
		}
      

        if (redraw && al_is_event_queue_empty(event_queue)) 
		{
            redraw = false;
            al_clear_to_color(al_map_rgb(0,0,0));

			// FIRST SCREEN TO-DO

			/*
			if(gamePaused == true)
			{
			first_screen_pos--;
			if(first_screen_pos == -1022)
			{
				first_screen_pos = 0;
			}
			al_draw_bitmap(first_screen,first_screen_pos,0,NULL);
			al_draw_text(font, al_map_rgb(255,255,255), SCREEN_W/2, 200 ,ALLEGRO_ALIGN_CENTRE, "Singleplayer");

			}
			*/

			if(gamePaused == false)
			{

			//draw the ground
            //note that drawing is from the top-left corner,
            //and NOT the center as it is in Box2D.

			//the center
            b2Vec2 g_pos = groundBody->GetPosition(); 

            //we need to get the top left corner from this center
            float g_x = g_pos.x - g_width/2;
            float g_y = g_pos.y - g_height/2;

            //now convert the coordinates and draw
            al_draw_bitmap(ground, g_x*PIX_METER, g_y*PIX_METER, 0);

            //do the same for the other boxes
            b2Vec2 b_pos = body->GetPosition();
			b2Vec2 b_pos2 = body2->GetPosition();
			b2Vec2 b_pos_ball = body5->GetPosition();
			b2Vec2 b_pos3[100] = {};
			for(int w = 0; w<howManyBodies; w++)
			{
				b_pos3[w] = bodies[w]->GetPosition();
			}
            float32 angle = body->GetAngle();
			float32 angle2 = body2->GetAngle();
			float32 angle_ball = body5->GetAngle();
			float32 angle3[100] = {};
			for(int w = 0; w<howManyBodies; w++)
			{
				angle3[w] = bodies[w]->GetAngle();
			}

            //When using the rotated drawing function, the center is pinned to a point.
            //so essentially, we can draw from the center.
            float b_x = b_pos.x;
            float b_y = b_pos.y;
			float b_x2 = b_pos2.x;
            float b_y2 = b_pos2.y;
			float b_x_ball = b_pos_ball.x;
			float b_y_ball = b_pos_ball.y;

			// BULLET TIME EFFECT
			/*
			if(abs(b_pos.x-b_pos2.x)*PIX_METER < 100 && abs(b_pos.y-b_pos2.y)*PIX_METER < 100)
			{
				timeFactor = 5 ;
				timeStep = 1.0f/timeFactor/FPS;
				//printf("slow");
			}
			else
			{
				timeFactor = 1;
				timeStep = 1.0f/timeFactor/FPS;
			}
			*/

			// Positions for the small boxes
			for(int w = 0; w<howManyBodies; w++)
			{
				position_x[w] = b_pos3[w].x;
				position_y[w] = b_pos3[w].y;
			}

			// Center points for boxes/balls
            float midx = al_get_bitmap_width(player)/2;
            float midy = al_get_bitmap_height(player)/2;
			float bmidx = al_get_bitmap_width(smallbox)/2;
		    float bmidy = al_get_bitmap_height(smallbox)/2;
			float ball_mid_x = al_get_bitmap_width(ball)/2;
			float ball_mid_y = al_get_bitmap_height(ball)/2;

			// DRAW ALL
			
			backpos_x += velocity.x;
			backpos_y -= velocity.y;

			//al_draw_bitmap(background,backpos_x-(velocity.x),backpos_y-velocity.y,NULL);
			al_draw_bitmap(background,0,0,NULL);
			al_draw_bitmap(platform,0,704,NULL);
			al_draw_text(font, al_map_rgb(255,255,255), 32, 32 ,ALLEGRO_ALIGN_LEFT, "Currently spawning: ");
			if(itemSpawning == 0)
			{
				al_draw_text(font, al_map_rgb(255,255,255), 500, 32 ,ALLEGRO_ALIGN_LEFT, spawnTypes[0]);
			}
			if(itemSpawning == 1)
			{
				al_draw_text(font, al_map_rgb(255,255,255), 500, 32 ,ALLEGRO_ALIGN_LEFT, spawnTypes[1]);
			}
			if(itemSpawning == 2)
			{
				al_draw_text(font, al_map_rgb(255,255,255), 500, 32 ,ALLEGRO_ALIGN_LEFT, spawnTypes[2]);
			}
			al_draw_rotated_bitmap(player, midx, midy, b_x*PIX_METER, b_y*PIX_METER, angle, 0);
			al_draw_rotated_bitmap(player, midx, midy, b_x2*PIX_METER, b_y2*PIX_METER, angle2, 0);
			al_draw_rotated_bitmap(ball, ball_mid_x, ball_mid_y, b_x_ball*PIX_METER, b_y_ball*PIX_METER, angle_ball,0);
			for(int w = 0; w<howManyBodies; w++)
			{
				al_draw_rotated_bitmap(smallbox, bmidx, bmidy, position_x[w]*PIX_METER, position_y[w]*PIX_METER, angle3[w], 0);
				/*
				if(bodies[w]->GetPosition().y > 500/PIX_METER)
				{
					printf("ulkona ruudusta");
					world.DestroyBody(bodies[w]);				// ATTEMPT AT DESTROYING BODIES
					bodies[w] = NULL;
					bodies[w] = bodies[w+1];
					howManyBodies--;
					position_y[w] = position_y[w+1];
					position_x[w] = position_x[w+1];

				}
				*/
			}



			}
            al_flip_display();
        }

  }

  // When the world destructor is called, all bodies and joints are freed. This can
  // create orphaned pointers, so be careful about your world management.

  return 0;
}