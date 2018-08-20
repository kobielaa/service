<?php

/**
 * Access restriction.
 * @return bool
 *     Returns boolen
 */
function _timian_service_data_access($user)
{
    $security_key = 'uj38djdncnffjeidncnci9eqieruifewejhca8ujcuewdfuhuwf';
    return ( $security_key == $user['key']) ? true : false;
}

/**
 * Callback function
 */
function _timian_service_fetch_data($user)
{

    $user = user_load_by_name($user['name']);
    error_log(print_r($user,1),0);
    error_log(print_r($user['name'],1),0);
    if ($user) {
        error_log(print_r($user,1),0);
        return 'User : ' . $name . ' already exist';
    } else {
        error_log(print_r('chuje muje',1),0);
        // $new_user = [
        //     'name'   => $name,
        //     'pass'   => 'admin', // note: do not md5 the password
        //     'mail'   => 'test@gmail.com',
        //     'status' => 1,
        //     'init'   => 'test@gmail.com',
        //     'roles'  => [
        //         DRUPAL_AUTHENTICATED_RID => 'authenticated user',
        //         12                       => 'suppliers',
        //     ],
        // ];
        // user_save('', $new_user);
        return 'Created user : ' . $name;
    }
}