<?php

/**
 * Access restriction.
 * @return bool
 *     Returns boolen
 */
function _timian_service_data_access($data)
{
    // $user = user_load_by_name($user['name']);
    // $security_key = 'uj38djdncnffjeidncnci9eqieruifewejhca8ujcuewdfuhuwf';
    // return ( $security_key == $user['key']) ? true : false;
    return true;
}

function timian_service_get_single_order($data){
    error_log(print_r('weszlo',1),0);
    $order_data = [
		"order" => 'cokolwiek',

	];

    return $order_data;
}

