/* to be used with Arducube HW 0.2 */

/* number of LEDs the cube has */
#define LEDS_X 4
#define LEDS_Y 4
#define LEDS_Z 4

/* image, which should be shown */
byte image[LEDS_Z][LEDS_Y][LEDS_X] = {
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	},
	{
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0},
		{0,0,0,0}
	}
};

/* layer the timer is currently processing */
byte current_layer = 0;

/* array with sorted leds */
const byte led[LEDS_X*LEDS_Y]={5,7,13,12,16,15,3,8,17,14,2,4,18,19,1,0};

/* array with sorted layers */
const byte layer[LEDS_Z]={11,10,9,6};

void setup() {
	/* setup layers */
	for (int i=0;i<4;i++) {
		pinMode(layer[i],OUTPUT);
		digitalWrite(layer[i],LOW); 
	}

	/* setup leds */
	for (int i=0;i<16;i++) {
		pinMode(led[i],OUTPUT);
		digitalWrite(led[i],LOW); 
	}

	randomSeed(analogRead(0));

	/* enable timer 1A */
	TIMSK1 = _BV(OCIE1A);

	/* clock / 1024, 0.001024 seconds per tick, Mode=CTC */
	TCCR1A = _BV(CS12) | _BV(CS10) | _BV(WGM12);

	/* 0.001024*10 ~= .01, TIMER0_COMPA_vect will be triggered ca. 100 times per second */
	OCR1A = 10;
}

void reset() {
	for(int z=0; z < LEDS_Z; z++) {
		for(int y=0; y < LEDS_Y; y++) {
			for(int x=0; x < LEDS_X; x++) {
				image[z][y][x] = 0;
			}
		}
	}
}

void animation_pixel() {
	for(int z=0; z<LEDS_Z; z++) {
		for(int y=0; y<LEDS_Y; y++) {
			for(int x=0; x<LEDS_X; x++) {
				image[z][y][x] = 1;
				delay(10);
				image[z][y][x] = 0;
			}
		}
	}
}

void shift_down() {
	for(int z=3; z>0; z--) {
		for(int y=0; y<LEDS_Y; y++) {
			for(int x=0; x<LEDS_X; x++) {
				image[z][y][x] = image[z-1][y][x];
			}
		}
	}

	for(int y=0; y<LEDS_Y; y++) {
		for(int x=0; x<LEDS_X; x++) {
			image[0][y][x] = 0;
		}
	}
}

void shift_up() {
	for(int z=0; z<LEDS_Z-1; z++) {
		for(int y=0; y<LEDS_Y; y++) {
			for(int x=0; x<LEDS_X; x++) {
				image[z][y][x] = image[z+1][y][x];
			}
		}
	}

	for(int y=0; y<LEDS_Y; y++) {
		for(int x=0; x<LEDS_X; x++) {
			image[LEDS_Z-1][y][x] = 0;
		}
	}
}

void random_layer(byte layer, byte percent) {
	for(int y=0; y<LEDS_Y; y++) {
		for(int x=0; x<LEDS_X; x++) {
			image[layer][y][x] = (random(0,100) < percent);
		}
	}
}

void animation_rain() {
	reset();

	for(int i=0; i<100; i++) {
		shift_down();
		random_layer(0, 20);
		delay(100);
	}

	reset();
}

void animation_all_random() {
	reset();

	for(int i=0; i<100; i++) {
		random_layer(0, 25);
		random_layer(1, 25);
		random_layer(2, 25);
		random_layer(3, 25);
		delay(100);
	}
}

void animation_pge() {
	int state = 0;
	int max_loop = 25;

	for(int loop=0; loop<=max_loop; loop++) {
		// State 0
		// 1111
		// 1001
		// 1001
		// 1111
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(z != 0 && z != 3 && (y==1 || y==2) && (x==1 || x==2)) {
						image[z][y][x] = state;
					}
					else {
						image[z][y][x] = !state;
					}
				}
			}
		}
		delay(200);
		state = !state;
	}
}

void animation_falling() {
	reset();
	for(int i=0; i<64; i++) {
		byte newx, newy;
		do {
			newx = random(4);
			newy = random(4);
		} while(image[0][newy][newx] == 1);

		image[0][newy][newx] = 1;

		delay(100);

		for(byte z = 1; z < LEDS_Z; z++)	{
			if(image[z][newy][newx] == 0) {
				image[z][newy][newx]   = 1;
				image[z-1][newy][newx] = 0;
				delay(100);
			}
		}
	}
}

void animation_updown() {
	reset();
	int ms = 50;
	for(int loop=0; loop<=15; loop++) {
		for(int y=0; y<LEDS_Y; y++)
			for(int x=0; x<LEDS_X; x++)
				image[0][y][x] = 1;
		delay(ms);

		for(int i=3; i>0; i--) {
			shift_down();
			delay(ms);
		}

		for(int i=3; i>0; i--) {
			shift_up();
			delay(ms);
		}
	}
}

void animation_pge_rotation() {
	int max_loop = 10;
	int state = 0;
	for(int loop=0; loop<=max_loop; loop++) {
		for(int line_y=0; line_y<=3; line_y++) {
			for(int line_x=0; line_x<=3; line_x++) {
				if(image[0][0][3] == 1) {
					line_y=1;
					line_x=3;
				}
				else if(image[0][1][3] == 1) {
					line_y=2;
					line_x=3;
				}
				else if(image[0][2][3] == 1) {
					line_y=3;
					line_x=3;
				}
				else if(image[0][3][3] == 1) {
					line_y=3;
					line_x=2;
				}
				else if(image[0][3][2] == 1) {
					line_y=3;
					line_x=1;
				}
				else if(image[0][3][1] == 1) {
					line_y=3;
					line_x=0;
				}
				else if(image[0][3][0] == 1) {
					line_y=2;
					line_x=0;
				}
				else if(image[0][2][0] == 1) {
					line_y=1;
					line_x=0;
				}
				else if(image[0][1][0] == 1) {
					line_y=0;
					line_x=0;
				}
				for(int z=0; z<LEDS_Z; z++) {
					for(int y=0; y<LEDS_Y; y++) {
						for(int x=0; x<LEDS_X; x++) {
							if(y==line_y && x==line_x) {
								image[z][y][x] = 1;
							}
							else {
								image[z][y][x] = 0;
							}
						}
					}
				}
				delay(100);
			}
		}
	}
}
void animation_pge_flat_slow() {
	// delay
	int ms = 100;
	int max_loop = 3;

	for(int loop=0; loop<=max_loop; loop++) {
		//00-03
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(y==0) {
						image[z][y][x] = 1;
					}
					else {
						image[z][y][x] = 0;
					}
				}delay(ms);
			}
		}
		//03-33
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(x==3) {
						image[z][y][x] = 1;
					}
					else {
						image[z][y][x] = 0;
					}
				}delay(ms);
			}
		}
		//33-30
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(y==3) {
						image[z][y][x] = 1;
					}
					else {
						image[z][y][x] = 0;
					}
				}delay(ms);
			}
		}
		//30-00
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(x==0) {
						image[z][y][x] = 1;
					}
					else {
						image[z][y][x] = 0;
					}
				}delay(ms);
			}
		}
	}
}
void animation_pge_flat_fast() {
	// delay
	int ms = 200;
	int max_loop = 5;

	for(int loop=0; loop<max_loop; loop++) {
		//00-03
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(y==0) {
						image[z][y][x] = 1;
					}
					else {
						image[z][y][x] = 0;
					}
				}
			}
		}
		delay(ms);
		//03-33
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(x==3) {
						image[z][y][x] = 1;
					}
					else {
						image[z][y][x] = 0;
					}
				}
			}
		}
		delay(ms);
		//33-30
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(y==3) {
						image[z][y][x] = 1;
					}
					else {
						image[z][y][x] = 0;
					}
				}
			}
		}
		delay(ms);
		//30-00
		for(int z=0; z<LEDS_Z; z++) {
			for(int y=0; y<LEDS_Y; y++) {
				for(int x=0; x<LEDS_X; x++) {
					if(x==0) {
						image[z][y][x] = 1;
					}
					else {
						image[z][y][x] = 0;
					}
				}
			}
		}
		delay(ms);
	}
}
void loop() {
	reset();
	animation_pixel();
	animation_rain();
	animation_pge();
	animation_falling();
	animation_updown();
	animation_pge_rotation();
	animation_all_random();
	animation_pge_flat_slow();
	animation_pge_flat_fast();
}

ISR(TIMER1_COMPA_vect) {
	/* disable last layer */
	digitalWrite(layer[current_layer], LOW);

	/* switch to next layer */
	current_layer += 1;
	current_layer %= LEDS_Z;

	/* set leds for current layer */
	for(byte y=0; y<LEDS_Y; y++) {
		for(byte x=0; x<LEDS_X; x++) {
			digitalWrite(led[y*LEDS_X+x], image[current_layer][y][x]);
		}
	}

	/* enable layer */
	digitalWrite(layer[current_layer], HIGH);

	/* reset timer */
	OCR1A = 0;
}
