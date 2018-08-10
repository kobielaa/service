<?php
//require_once DRUPAL_ROOT . "/" . drupal_get_path('module', 'ubercart') . '/uc_order/uc_order.module';
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

function _timian_service_resource_delete( $order_id, $product_id)
{
    uc_order_product_delete($product_id);
    $order = uc_order_load($order_id,true);
    $response = uc_order_save($order);	

    $data = [
        [
        'response' => $response,
        ],


    ];
    
    return $data;
}