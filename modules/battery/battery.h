#ifndef BATTERY_H
#define BATTERY_H

struct battery_charge {
	int level1;
	int level2;
};

struct battery_config {
	int resolution;			// the resolution used to sample the ADC (12, 14, 16 or 18 bits)
	int max_value;			// override the value that will be used to indicate 100%, when not set it will default to max_value for the resolution selected
	int num_samples;		// the number of samples that will be used to calculate an average
	int sample_interval;	// the number of ms between samples
};

#define BATT_THERMIOC_MAGIC		'B'
#define BATTERY_GET_CHARGE		_IOR(BATT_THERMIOC_MAGIC, 0, struct battery_charge)

#define BATTERY_GET_CONFIG		_IOR(BATT_THERMIOC_MAGIC, 2, struct battery_config)
#define BATTERY_SET_CONFIG		_IOW(BATT_THERMIOC_MAGIC, 3, struct battery_config)

int battery_get_charge(struct battery_charge* arg);
int battery_get_config(struct battery_config* cfg);
int battery_set_config(struct battery_config* cfg);

#endif // BATTERY_H
