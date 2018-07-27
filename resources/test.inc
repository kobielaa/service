<?php

/**
 * Access restriction.
 * @return bool
 *     Returns boolen
 */
function _timian_service_data_access($key)
{
    error_log(print_r($key,1),0);


    // $permission = FALSE;
    // global $user;
    // $roles = $user->roles;

    // if (in_array('administrator', $roles)){
    //     $permission = TRUE;
    // }
    return true;
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