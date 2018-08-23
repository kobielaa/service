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

/**
 * Callback function to create new supplier user
 *
 * @param array $data
 *   User's data from http post request
 *
 * @return array
 *   json with result
 */
function _timian_service_create_supplier($data)
{
    $user = user_load_by_name($data['name']);

    if ($user) {
        return [
            "result" => "fail"
        ];
    } else {
        //get the highest supplier value to avoide duplicate entry
        $query = db_select('field_data_field_user_supplier', 'n')->fields('n')->orderBy('field_user_supplier_value','DESC');
        $supplier_value = $query->execute()->fetchAll();
        $supplier_value = $supplier_value[0]->field_user_supplier_value + 1;

        $new_user = [
            'name'                => $data['name'],
            'pass'                => drupal_random_key(), // random password, user will be login trough sso
            'mail'                => $data['email'],
            'status'              => 1,
            'init'                => $data['email'],
            'roles'               => [
                DRUPAL_AUTHENTICATED_RID => 'authenticated user',
                12                       => 'suppliers',
            ],
            'field_user_fullname' => [
                'und' => [
                    0 => [
                        'value' => $data['name'],
                    ],
                ],
            ],
            'field_user_supplier' => [
                'und' => [
                    0 => [
                        'value' => $supplier_value,
                    ],
                ],
            ],
        ];
        user_save('', $new_user);

        // create node with typ birgi 
        create_node_birgi($data);

        return [
            "result" => "success"
        ];
    }
}

/**
 * Callback function for update user data
 *
 * @param array $data
 *   User's data from http post request
 *
 * @return array
 *   json with result
 */
function _timian_service_update_supplier($data)
{
    $node_nid = get_node_nid($data['name']);
    $user = user_load_by_name($data['name']);
    $node = node_load($node_nid);

    if ($node) {
        $node->field_supplier_defphone['und'][0]['value'] = $data['phone'];
        $node->field_supplier_sid['und'][0]['value'] = $data['sid'];
        $node->field_supplier_def_web['und'][0]['value'] = $data['webpage'];
        $node->field_supplier_order_email['und'][0]['value'] = $data['email_for_orders'];
        $node->field_supplier_def_email['und'][0]['value'] = $data['email'];
        $node->field_supplier_shorthand['und'][0]['value'] =  $data['short_name'];
        $node->field_min_order_total['und'][0]['value'] = $data['minimum'];
        $node->field_supplier_address1['und'][0]['value'] = $data['street'] . ", " . $data['postal_code'] . " " . $data['city'];
        $node->field_supplier_price_permission['und'][0]['value'] = 0;
        node_save($node);
    } else {
        create_node_birgi($data);
    }
    
    if ($user) {
        $user->mail = $data['email'];
        user_save($user);
    } else {
        _timian_service_create_supplier($data);
    }
}

/**
 * Callback function for checking accessibility of username
 *
 * @param array $data
 *   User's data from http post request
 *
 * @return array
 *   json with result
 */
function _timian_service_validate_supplier($data)
{
    $user = user_load_by_name($data['name']);

    return [
        "result" => $user ? "fail" : "success"
    ];
}

function _timian_service_delete_supplier($data)
{
    $node_nid = get_node_nid($data['name']);
    $user = user_load_by_name($data['name']);
    
    node_delete($node_nid);
    user_delete($user->uid);

}

function get_node_nid($node_title){
    $query = db_select('node', 'n')->fields('n')->condition('title', $node_title)->condition('type','birgi');
    $node_nid = $query->execute()->fetchAll();
    $node_nid = $node_nid[0]->nid;
    return $node_nid;
}

function create_node_birgi($data) {
    $node = new stdClass();
    $node->title = $data['name'];
    $node->type = "birgi";
    node_object_prepare($node); 
    $node->language = LANGUAGE_NONE; 
    $node->uid = 1; 
    $node->status = 1; 
    $node->promote = 1; 
    $node->comment = 2; 

    // Custom fields
    $node->field_supplier_defphone['und'][0]['value'] = $data['phone'];
    $node->field_supplier_sid['und'][0]['value'] = $data['sid'];
    $node->field_supplier_def_web['und'][0]['value'] = $data['webpage'];
    $node->field_supplier_order_email['und'][0]['value'] = $data['email_for_orders'];
    $node->field_supplier_def_email['und'][0]['value'] = $data['email'];
    $node->field_supplier_shorthand['und'][0]['value'] =  $data['short_name'];
    $node->field_min_order_total['und'][0]['value'] = $data['minimum'];
    $node->field_supplier_address1['und'][0]['value'] = $data['street'] . ", " . $data['postal_code'] . " " . $data['city'];
    $node->field_supplier_price_permission['und'][0]['value'] = 0;

    $node = node_submit($node);
    node_save($node);
}