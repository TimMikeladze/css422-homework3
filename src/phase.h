typedef enum {
	CPU_PHASE, IO_PHASE, FINISHED_PHASE
} PhaseType;

typedef struct Phase {
	PhaseType phaseType;
	int duration;
} Phase;

