

void set_eyelid_motor_speed(char eye, char dir, int speed);
int find_eyelid_limit(char eye, char dir);
int goto_eyelid_position(char eye, int speed);

void fast_eyelid_stop();
void fast_eyelid_stop_right();
void fast_eyelid_stop_left();