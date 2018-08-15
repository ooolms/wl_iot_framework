words=""

listIdentifiedDevices()
{
	words=`wliotproxy -compl list_identified`
}

listSensors()
{
	words=`wliotproxy -compl list_sensors "${dev}"`
}

listStoragesDevs()
{
	words=`wliotproxy -compl helper list_storages_devs`
}

listStoragesSensors()
{
	words=`wliotproxy -compl helper list_storages_sensors "${dev}"`
}

listExtrenalServices()
{
	words=`wliotproxy -compl helper list_external_services`
}

listDeviceCommands()
{
	words=`wliotproxy -compl list_commands "${dev}"`
}

processAddStorage()
{
	case $COMP_CWORD in
		2)
			listIdentifiedDevices
			;;
		3)
			dev="${COMP_WORDS[1]}"
			listSensors
			;;
		4)
			words="continuous auto_sessions manual_sessions last_n_values memory"
			;;
		5)
			words="dont_touch add_global_time drop_time"
			;;
	esac
}

processBindStorage()
{
	case $COMP_CWORD in
		2)
			listStoragesDevs
			;;
		3)
			dev="${COMP_WORDS[1]}"
			listStoragesSensors
			;;
		4)
			listExtrenalServices
			;;
	esac
}

processDevicesConfig()
{
	case $COMP_CWORD in
		2)
			words="get_tty_by_port_name set_tty_by_port_name get_tty_by_vid_pid set_tty_by_vid_pid get_tcp_by_address set_tcp_by_address set_detect_tcp_devices"
			;;
		3)
			if [[ "${COMP_WORDS[1]}" == "set_detect_tcp_devices" ]]
			then
				words="0 1"
			fi
			;;
	esac
}

processExecCommand()
{
	case $COMP_CWORD in
		2)
			listIdentifiedDevices
			;;
		3)
			dev="${COMP_WORDS[1]}"
			listDeviceCommands
			;;
	esac
}

_wliotproxy()
{
	COMPREPLY=()
	cur="${COMP_WORDS[COMP_CWORD-1]}"
	commands="add_storage bind_storage devices_config exec_command get_samples get_samples_count 
		identify_tty identify_tcp js_program list_commands list_identified list_sensors list_storages list_tty register_virtual_device remove_storage session vdev_meas"
	if [[ ${COMP_CWORD} == 1 ]] ; then
		COMPREPLY=( $(compgen -W "${commands}" -- ${cur}) )
	else
		cmd="${COMP_WORDS[0]}"
		case "$cmd" in
			"add_storage")
				processAddStorage
				;;
			"bind_storage")
				processBindStorage
				;;
			"devices_config")
				processDevicesConfig
				;;
			"exec_command")
				processExecCommand
				;;
		esac
		COMPREPLY=( $(compgen -W "${words}" -- "${cur}") )
	fi
}

complete -F _wliotproxy wliotproxy
