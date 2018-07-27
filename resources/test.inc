<?php

/**
 * Access restriction.
 * @return bool
 *     Returns boolen
 */
function _timian_service_data_access($key)
{

    $security_key = 'uj38djdncnffjeidncnci9eqieruifewejhca8ujcuewdfuhuwf';
    error_log(print_r($key,1),0);
    error_log(print_r($security_key,1),0);
    return ( $security_key == $key) ? true : false;
}

/**
 * Callback function
 */
function _timian_service_fetch_data($key, $name)
{
    $user = user_load_by_name($name);
    if ($user) {
        return 'User : ' . $name . ' already exist';
    } else {
        $new_user = [
            'name'   => $name,
            'pass'   => 'admin', // note: do not md5 the password
            'mail'   => 'test@gmail.com',
            'status' => 1,
            'init'   => 'test@gmail.com',
            'roles'  => [
                DRUPAL_AUTHENTICATED_RID => 'authenticated user',
                12                       => 'suppliers',
            ],
        ];
        user_save('', $new_user);
        return 'Created user : ' . $name;
    }
}