typedef enum {
	CPU_PHASE, IO_PHASE, FINISHED_PHASE
} PhaseType;

typedef struct Phase {
	PhaseType type;
	int duration;
} Phase;

