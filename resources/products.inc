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
        [
        'id' => 'Aragorn',
        'text' => 'Human'
        ],
        [
            'id' => 'Basia',
            'text' => 'mama'
        ],

    ];
    
       
    return $data;
}

function _timian_service_fetch_product($id)
{
    $data = [
        [
        'id' => $id,
        'text' => 'Human'
        ],
        [
            'id' => 'dupa',
            'text' => 'mama'
        ],

    ];
    
       
    return $data;
}

function _timian_service_resource_delete($id)
{

    $cokowliek = [
        [
        'id' => dirname(__FILE__) . '/uc_order.order_pane.inc',
        'text' => 'Human'
        ],
        [
            'id' => 'dupa',
            'text' => 'mama'
        ],

    ];
    
       
    return $cokowliek;
    
}