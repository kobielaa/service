<?php

/**
 * Access restriction.
 * @return bool
 *     Returns boolen
 */
function _timian_service_data_access($key)
{
    return true;
}

/**
 * Callback function
 */
function _timian_service_fetch_data($id)
{
    $data = [
        'name' => 'Aragorn',
        'race' => 'Human'
    ];
    
       
    return json_encode($data);
}