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
function _timian_service_fetch_data($cos){

    return 'blabla jest : ' . $cos ;
}