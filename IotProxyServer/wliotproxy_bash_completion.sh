_complete_devices()
{
	return `wliotproxy -compl list_identified`
}

_complete_devices()
{
	return `wliotproxy -compl list_identified`
}

_wliotproxy()
{
	COMPREPLY=()
	cur="${COMP_WORDS[COMP_CWORD]}"
	commands="add_storage bind_sensor devices_config exec_command get_samples get_samples_count identify_tty js_program list_commands list_identified list_sensors list_storages list_tty register_virtual_device remove_storage session_get_attr session_get_write_id session_list session_list_attrs session_set_attr session_start session_stop vdev_meas"
	if [[ ${COMP_CWORD} == 1 ]] ; then
		COMPREPLY=( $(compgen -W "${commands}" -- ${cur}) )
	else
		cmd="${COMP_WORDS[1]}"
		if [[ ${COMP_CWORD} == 2 ]] ; then
			if [[ "$cmd" == "list_sensors" || "$cmd" == "list_commands" || "$cmd" == "add_storage" ]] ; then
				words= _complete_devices
				COMPREPLY=( $(compgen -W "${words}" -- ${cur}) )
			fi
		fi
	fi
}

complete -F _wliotproxy wliotproxy
