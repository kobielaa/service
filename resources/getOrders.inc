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

function timian_service_get_single_order($supplier_name, $order_id){
    
    $args = func_get_args();
    error_log(print_r($args,1),0);
    $order_data = [
		"order_id" => $supplier_name,
        "supplier_name" => $order_id,
	];

    return $order_data;
}

