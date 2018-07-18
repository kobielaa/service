<?php

 /**
  * Access restriction.
  * @return bool
  *     Returns boolen
  */
  function _timian_service_data_access($op = 'config', $args = []) {
    // $permission = FALSE;
    // global $user;
    // $roles = $user->roles;

    // if (in_array('administrator', $roles)){
    //     $permission = TRUE;
    // }
    return TRUE; 
}

 /**
  * Callback function
  */
function _timian_service_fetch_data($name){
    $new_user = array(
        'name' => $name,
        'pass' => 'admin', // note: do not md5 the password
        'mail' => 'gowniany@gmail.com',
        'status' => 1,
        'init' =>  'gowniany@gmail.com',
        'roles' => array(
          DRUPAL_AUTHENTICATED_RID => 'authenticated user',
          12 => 'suppliers',
        ),
      );
      
      // The first parameter is sent blank so a new user is created.
     $uid =  user_save('', $new_user);
      error_log(print_r($uid,1),0);
    return 'blabla jest : ' . $name ;
}