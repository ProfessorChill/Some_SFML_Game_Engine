class Movement
{
    public:
	float acceleration = 0.1f;
	float deceleration = 0.25f;
	float maxWalkSpeed = 2.0f;
	float maxRunSpeed = 4.0f;

	bool movementUp;
	bool movementDown;
	bool movementLeft;
	bool movementRight;
	bool running;
};
