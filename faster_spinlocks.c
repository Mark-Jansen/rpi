
// this data is protected with the spinlock.
spinlock g_Lock;
pwm_config g_SharedConfig;
int g_Changed = 1;		// !important: we want to make sure the shared config is copied initially!

// this data is not protected with the spinlock,
// but the !only! one that can write to it is the timer_cb
pwm_config g_PrivateConfig;


void timer_cb()
{
	// enqueue the new timer before fiddling with the lock!
	mod_timer( &g_Timer, .... );
	// now we can check the modified flag, and if it is 0 we do not have to take the lock :)
	if( g_Changed ) {
		// someone altered the shared data, so we need to read it.
		// first we aquire the lock, to be sure noone is writing it anymore.
		spin_lock_xx( g_Lock );
		// after we aquire the lock we can copy the shared (locked) config to the private (unlocked) copy
		memcpy( &g_PrivateConfig, &g_SharedConfig, sizeof(g_PrivateConfig) );
		// and mark the shared config as 'read'
		g_Changed = 0;
		spin_unlock_xx( g_Lock );
	}
	
	// only use the g_PrivateConfig here, not the g_SharedConfig!
}

// only use the g_SharedConfig here, and only alter it while holding the lock!
// if the g_SharedConfig is modified, also set the modified flag !while holding the same lock!
void set_config( pwm_config* newConfig )
{
	spin_lock_xx( g_Lock );
	memcpy( &g_SharedConfig, newConfig, sizeof(g_SharedConfig) );
	g_Changed = 1;
	spin_unlock_xx( g_Lock );
}
