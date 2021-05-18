struct Button_State {
	bool is_down;
	bool is_changed;
};

enum {
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_RIGHT,
	BUTTON_LEFT,
	BUTTON_ENTER,
	BUTTON_W,
	BUTTON_S,
	BUTTON_B,
	BUTTON_A,
	BUTTON_D,

	BUTTON_COUNT //Should be the last item
};

struct Input {
	Button_State buttons[BUTTON_COUNT];
};
