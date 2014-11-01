#ifndef BATTERY_H
#define BATTERY_H

struct battery_charge {
	int level1;
	int level2;
};

struct battery_config {
	int resolution;			// the resolution used to sample the ADC (12, 14, 16 or 18 bits)
	int num_samples;		// the number of samples that will be used to calculate an average
	int sample_interval;	// the number of ms between samples
};

#define THERMIOC_MAGIC			'B'
#define BATTERY_GET_CHARGE		_IOR(THERMIOC_MAGIC, 0, struct battery_charge)

#define BATTERY_GET_CONFIG		_IOR(THERMIOC_MAGIC, 2, struct battery_config)
#define BATTERY_SET_CONFIG		_IOW(THERMIOC_MAGIC, 3, struct battery_config)

int battery_get_charge(struct battery_charge* arg);
int battery_get_config(struct battery_config* cfg);
int battery_set_config(struct battery_config* cfg);

#endif // BATTERY_H
