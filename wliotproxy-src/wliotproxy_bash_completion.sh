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

listCommands()
{
	words=`wliotproxy -compl list_commands "${dev}"`
}

listJsPrograms()
{
	words=`wliotproxy -compl js_program list`
}

listVirtualDevices()
{
	words=`wliotproxy -compl helper list_virtual_devices`
}

listSessions()
{
	words=`wliotproxy -compl session list "${dev}" "${sensor}"`
}

listAvailableDataExportServices()
{
	words=`wliotproxy -compl available_data_export_services`
}

listStorageDataExportServices()
{
	words=`wliotproxy -compl data_export list "${dev}" "${sensor}"`
}

processAddStorage()
{
	case $COMP_CWORD in
		2)
			listIdentifiedDevices
			;;
		3)
			dev="${COMP_WORDS[2]}"
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

processDevicesConfig()
{
	case $COMP_CWORD in
		2)
			words="get_tty_by_port_name set_tty_by_port_name get_tty_by_vid_pid set_tty_by_vid_pid get_tcp_by_address set_tcp_by_address set_detect_tcp_devices"
			;;
		3)
			if [[ "${COMP_WORDS[2]}" == "set_detect_tcp_devices" ]]
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
			dev="${COMP_WORDS[2]}"
			listCommands
			;;
	esac
}

processFindStorage()
{
	case $COMP_CWORD in
		2)
			listStoragesDevs
			;;
		3)
			dev="${COMP_WORDS[2]}"
			listStoragesSensors
			;;
	esac
}

processIdentifyTty()
{
	if [[ $COMP_CWORD == 2 ]]
	then
		cd /dev
		words=`ls -x tty*`
		cd - 1>/dev/null
	fi
}

processJsProgram()
{
	case $COMP_CWORD in
		2)
			words="list start stop restart"
			;;
		3)
			subCmd=${COMP_WORDS[2]}
			if [[ "$subCmd" == "start" || "$subCmd" == "stop" || "$subCmd" == "restart" ]]
			then
				listJsPrograms
			fi
			;;
	esac
}

processDataExportCommand()
{
	case $COMP_CWORD in
		2)
			words="list get add remove"
			;;
		3)
			listStoragesDevs
			;;
		4)
			dev="${COMP_WORDS[3]}"
			listStoragesSensors
			;;
		5)
			subCmd=${COMP_WORDS[2]}
			if [[ "$subCmd" == "add" ]]
			then
				listAvailableDataExportServices
			elif [[ "$subCmd" == "get" || "$subCmd" == "remove" ]]
			then
				dev="${COMP_WORDS[3]}"
				sensor="${COMP_WORDS[4]}"
				listStorageDataExportServices
			fi
			;;
	esac

}

processSession()
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
			words="list list_attr get_attr set_attr start stop continue get_write_id"
			;;
		5)
			dev="${COMP_WORDS[2]}"
			sensor="${COMP_WORDS[3]}"
			subCmd="${COMP_WORDS[4]}"
			if [[ "$subCmd" == "list_attr" || "$subCmd" == "get_attr" || "$subCmd" == "set_attr" || "$subCmd" == "continue" ]]
			then
				listSessions
			fi
			;;
	esac

}

processApm()
{
	case $COMP_CWORD in
		2)
			words="user grp dev"
			;;
		3)
			subCmd="${COMP_WORDS[2]}"
			case $subCmd in
				"user")
					words="create passwd chpass del list"
					;;
				"grp")
					words="list create del add_user del_user list_users get_moder set_moder"
					;;
				"dev")
					words="list list_rules set_rule chown"
					;;
			esac
			;;
	esac
}

_wliotproxy()
{
	COMPREPLY=()
	cur="${COMP_WORDS[COMP_CWORD]}"
	words=""
	commands="add_storage devices_config exec_command get_samples get_samples_count identify_server data_export
		identify_tcp identify_tty js_program list_commands list_identified list_sensors list_storages list_tty
		remove_storage session available_data_export_services change_device_owner apm"
	if [[ $COMP_CWORD == 1 ]] ; then
		COMPREPLY=( $(compgen -W "${commands}" -- ${cur}) )
	else
		cmd="${COMP_WORDS[1]}"
		case "$cmd" in
			"add_storage")
				processAddStorage
				;;
			"devices_config")
				processDevicesConfig
				;;
			"exec_command")
				processExecCommand
				;;
			"data_export")
				processDataExportCommand
				;;
			"get_samples"|"get_samples_count"|"remove_storage")
				processFindStorage
				;;
			"identify_tty")
				processIdentifyTty
				;;
			"js_program")
				processJsProgram
				;;
			"list_commands"|"list_sensors"|"change_device_owner")
				if [[ $COMP_CWORD == 2 ]]
				then
					listIdentifiedDevices
				fi
				;;
			"session")
				processSession
				;;
			"apm")
				processApm
				;;
		esac
		COMPREPLY=( $(compgen -W "${words}" -- "${cur}") )
	fi
}

complete -F _wliotproxy wliotproxy
