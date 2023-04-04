
	/*
	 * adios2 object declared before loop entered
	 * only when ioLib chosen is adios2 and one of its engines 
	 */ 
	if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
	{
		adios2_adios *adios = adios2_init_config_mpi(config_file, cartcomm); // cartcomm); // if using ADIOS2 MPI, need to include debugger. 
	} 
