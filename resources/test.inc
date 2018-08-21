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
 * Callback function
 */
function _timian_service_fetch_data($data)
{
    //error_log(print_r($data,1),0);
    $user = user_load_by_name($data['name']);
    error_log(print_r($user, 1), 0);
    // error_log(print_r($user,1),0);
    // error_log(print_r($user['name'],1),0);
    if ($user) {
        foreach ($data as $key => $value) {
            if ($value != "" && $value != null) {
                switch ($key) {
                    case 'phone':
                        $user->field_phone_nr['und'][0]['value'] = $value;
                        break;
                    case 'sid':
                        $user->field_supplier_sid['und'][0]['value'] = $value;
                        break;
                    case 'webpage':
                        $user->field_supplier_def_web['und'][0]['value'] = $value;
                        break;
                    case 'email':
                        $user->field_supplier_order_email['und'][0]['value'] = $value;
                        break;
                    case 'short_name':
                        $user->field_supplier_shorthand['und'][0]['value'] = $value;
                        break;
                    case 'minimum':
                        $user->field_min_order_total['und'][0]['value'] = $value;
                        break;
                    // case 'short_name':
                    //     $user->field_min_order_total['und'][0]['value'] = $value;
                    //     break;
                }
            }
        }
        if ($data['street'] != "" && $data['postal_code'] != "" && $data['city'] != "") {
            $user->field_supplier_address1['und'][0]['value'] = $data['street'] . ", " . $data['postal_code'] . " " . $data['city'];
        }
        user_save($user);
        error_log(print_r($user, 1), 0);
        return 'Successfully updated user : ' . $data['name'];
    } else {
        error_log(print_r('chuje muje', 1), 0);
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
        ];
        user_save('', $new_user);

        //insert data only visible for user with supplier role
        $user = user_load_by_name($data['name']);
        error_log(print_r($user, 1), 0);
        $user->field_phone_nr['und'][0]['value'] = $data['phone'];
        $user->field_supplier_sid['und'][0]['value'] = $data['sid'];
        $user->field_supplier_def_web['und'][0]['value'] = $data['webpage'];
        $user->field_supplier_order_email['und'][0]['value'] = $data['email'];
        $user->field_supplier_shorthand['und'][0]['value'] = $data['short_name'];
        $user->field_min_order_total['und'][0]['value'] = $data['minimum'];
        user_save($user);

        $user = user_load_by_name($data['name']);
        error_log(print_r($user, 1), 0);
        return 'Created user : ' . $data['name'];
    }
}