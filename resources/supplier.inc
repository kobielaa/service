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

function _timian_service_create_user($data)
{
    $user = user_load_by_name($data['name']);

    if ($user) {
        if (!$user->status) {
            $user->status = "1";
            user_save($user);
            header("HTTP/1.0 201");
            return [
                "result" => "User {$data['name']} activated",
            ];

        } else {
            header("HTTP/1.0 409");
            return [
                "result" => "User {$data['name']} already exists",
            ];
        }

    } else {

        $supplier_value = get_node_nid($data['birgi']); //get_node_nid_by_acronym($data['birgi']);

        $new_user = [
            'name' => $data['name'],
            'pass' => drupal_random_key(), // random password, user will be login trough sso
            'mail' => $data['email'],
            'status' => 1,
            'init' => $data['email'],
            'roles' => [
                DRUPAL_AUTHENTICATED_RID => 'authenticated user',
                12 => 'suppliers',
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

        header("HTTP/1.0 201");
        return [
            "result" => "User {$data['name']} created",
        ];
    }
}

function _timian_service_create_supplier_entity($data)
{
    $name_query = new EntityFieldQuery();

    if (array_key_exists('previous_supplier_name', $data) && !empty($data['previous_supplier_name'])) {
        $name = $name_query->entityCondition('entity_type', 'node')
            ->propertyCondition('type', 'birgi')
            ->propertyCondition('title', $data['previous_supplier_name'])
            ->count()->execute();

        if ($name) { //if had previous name
            _timian_service_update_supplier_entity($data);
            header("HTTP/1.0 201");
            return [
                "result" => "Supplier {$data['name']} updated",
            ];
        }
    } else {
        $name = $name_query->entityCondition('entity_type', 'node')
            ->propertyCondition('type', 'birgi')
            ->propertyCondition('title', $data['name'])
            ->count()->execute();

        if ($name) { //if already exist updated
            $data['previous_supplier_name'] = $data['name'];
            _timian_service_update_supplier_entity($data);
            header("HTTP/1.0 201");
            return [
                "result" => "Supplier {$data['name']} updated",
            ];
        }
    }

    create_node_birgi($data);
    header("HTTP/1.0 201");
    return [
        "result" => "Supplier {$data['name']} created",
    ];
}
/**
 * Callback function to update new supplier user
 *
 * @param array $data
 *   User's data from http post request
 *
 * @return array
 *   json with result
 */
function _timian_service_update_supplier($data)
{
    $user = user_load_by_name($data['user']);

    if ($user) {

        $supplier_value = get_node_nid($data['birgi']);

        $update_user = [
            'name' => $data['name'],
            'mail' => $data['email'],
            'init' => $data['email'],
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

        user_save($user, $update_user);
        header("HTTP/1.0 200");

        return [
            "result" => "User {$data['user']} updated",
        ];
    } else {
        header("HTTP/1.0 404");
        return [
            "result" => "User {$data['user']} not found",
        ];
    }
}

// /**
//  * Callback function for update user data
//  *
//  * @param array $data
//  *   User's data from http post request
//  *
//  * @return array
//  *   json with result
//  */
function _timian_service_update_supplier_entity($data)
{
    $node_nid = get_node_nid($data['previous_supplier_name']);
    $node = node_load($node_nid);

    if ($node) {
        $node->title = $data['name'];
        $node->status = "1";
        $node->field_supplier_defphone['und'][0]['value'] = $data['phone'];
        $node->field_supplier_sid['und'][0]['value'] = $data['sid'];
        $node->field_supplier_def_web['und'][0]['value'] = $data['webpage'];
        $node->field_supplier_order_email['und'][0]['value'] = $data['email_for_orders'];
        $node->field_supplier_def_email['und'][0]['value'] = $data['email'];
        $node->field_supplier_shorthand['und'][0]['value'] = $data['short_name'];
        $node->field_min_order_total['und'][0]['value'] = $data['minimum'];
        $node->field_supplier_address1['und'][0]['value'] = $data['street'] . ", " . $data['postal_code'] . " " . $data['city'];
        $node->field_supplier_price_permission['und'][0]['value'] = 0;
        $node->field_supplier_can_upload['und'][0]['value'] = $data['can_upload'];

        node_save($node);

        header("HTTP/1.0 200");

        return [
            "result" => "Supplier {$data['name']} updated",
        ];
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
function _timian_service_validate_user($data)
{
    $user = user_load_by_name($data['name']);

    return [
        "result" => $user ? "fail" : "success",
    ];
}

// /**
//  * Callback function for checking accessibility of username
//  *
//  * @param array $data
//  *   User's data from http post request
//  *
//  * @return array
//  *   json with result
//  */
// function _timian_service_validate_user($data)
// {
//     $operation = $data["operation"];
//     $user = user_load_by_name($data['name']);
//     $emptyData = "";

//     if (empty($data['name'])) { //validate parameters
//         $emptyData .= "name, ";
//     }
//     if (empty($data['operation'])) {
//         $emptyData .= "operation, ";
//     }
//     if ($data['operation'] == "update") {
//         if(!array_key_exists('newUserName',$data) || empty($data['newUserName']))
//         $emptyData .= "newUserName, ";
//     }

//     if (!empty($emptyData)) {
//         header("HTTP/1.0 400");

//         return ["message" => "Following data was not provided: {$emptyData}", "status" => 400];
//     } else {

//         switch (strtolower($operation)) {
//             case 'create':
//                 if (!empty($user)) {
//                     return ["result" => "fail", "message" => "User {$data['name']} already exist"];
//                 }
//                 return ["result" => "success"]; //checked above if exist
//             case 'update':
//                 if (!empty($user)) {
//                     $newUserName = $data['newUserName'];
//                     if (!user_load_by_name($newUserName) || $data['name'] == $newUserName) {
//                         return ["result" => "success"];
//                     } else {
//                         return ["result" => "fail", "message" => "User name {$newUserName} already exist"];
//                     }
//                 } else {
//                     return ["result" => "fail", "message" => "User {$data['name']} not found"];
//                 }
//             default:
//                 header("HTTP/1.0 400");
//                 return ["message" => "Wrong type of operation: {$operation}", "status" => 400];
//         }
//     }

// }

function _timian_service_validate_supplier($data)
{
    $name_query = new EntityFieldQuery();
    $name = $name_query
        ->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'birgi')
        ->propertyCondition('status', array(0, 1), 'IN')
        ->propertyCondition('title', $data['name'])
        ->count()->execute();

    $sid = db_query("SELECT field_supplier_sid_value 
    FROM `main_field_data_field_supplier_sid`
    where field_supplier_sid_value = :sid", 
    array(':sid' => $data['sid']))->fetchField();

    $acronym = db_query("SELECT field_supplier_shorthand_value 
    FROM `main_field_data_field_supplier_shorthand`
    where field_supplier_shorthand_value = :acr", 
    array(':acr' => $data['acronym']))->fetchField();

    return [
        "name" => $name ? 'not-unique' : 'unique',
        "sid" => $sid ? 'not-unique' : 'unique',
        "acronym" => $acronym ? 'not-unique' : 'unique',
    ];
}

function _timian_service_delete_supplier($data)
{
    $node_nid = get_node_nid($data['name']);
    if ($node_nid) {
        $node = node_load($node_nid);
        $node->status = "0";
        node_save($node);

        $query = new EntityFieldQuery();
        $query->entityCondition('entity_type', 'user')
            ->fieldCondition('field_user_supplier', 'value', $node_nid, '=');
        $result = $query->execute();
        $users_ids = array_keys($result['user']);

        foreach ($users_ids as $id) {
            $user = user_load($id);
            $user->status = 0;
            user_save($user);
        }
        header("HTTP/1.0 201");

        return [
            "result" => "Supplier {$data['name']} blocked",
        ];
    }

}

function get_node_nid($node_title)
{
    $query = db_select('node', 'n')->fields('n')->condition('title', $node_title)->condition('type', 'birgi');
    $node_nid = $query->execute()->fetchAll();
    $node_nid = $node_nid[0]->nid;
    return $node_nid;
}

function get_node_nid_by_acronym($birgi)
{
    $query = db_select('field_data_field_supplier_shorthand', 'n');
    $query->addField('n', 'entity_id', 'id');
    $query->condition('field_supplier_shorthand_value', $birgi, '=');
    $node_nid = $query->execute()->fetchAll();

    return $node_nid ? $node_nid[0]->id : "_none";
}

function create_node_birgi($data)
{
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
    $node->field_supplier_shorthand['und'][0]['value'] = $data['short_name'];
    $node->field_min_order_total['und'][0]['value'] = $data['minimum'];
    $node->field_supplier_address1['und'][0]['value'] = $data['street'] . ", " . $data['postal_code'] . " " . $data['city'];
    $node->field_supplier_price_permission['und'][0]['value'] = 0;
    $node->field_supplier_can_upload['und'][0]['value'] = $data['can_upload'];


    $node = node_submit($node);
    node_save($node);
}
