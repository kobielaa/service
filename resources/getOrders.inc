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

function timian_service_get_single_order($supplier_name, $order_id){
    
    $uc_order = uc_order_load($order_id);
    set_supplier_info_for_order($uc_order);
    if($uc_order->supplier_name == $supplier_name) {
        $order = new timianOrder($uc_order);
        if (is_array($order) || is_object($order)) {
            header('Content-Type: application/json');
            return json_encode($order);
        }
    }
    else {
        return header("HTTP/1.0 415 No orders for supplier");
    }
}
function set_supplier_info_for_order($order) {
	if (isset($order->order_id)) {
		$query = db_select("eldhus_uc_order_fields","o");
        $query->innerJoin('field_data_field_user_supplier','s','o.supplier_id=s.field_user_supplier_value');
        $query->innerJoin('field_data_field_supplier_sid','r','o.supplier_id=r.entity_id');
        $query->innerJoin('users','u','s.entity_id=u.uid');
        $query->fields("s")->fields("u")->fields("o")->fields("r");
        $query->condition("o.order_id", $order->order_id);
       
		$supplier = $query->execute()->fetch();

        if ($supplier) {
			$order->supplier_name = $supplier->name;
			$order->supplier_sid = $supplier->field_supplier_sid_value;
		}
		else {
			$order->supplier_name = t('Supplier not found');
			$order->supplier_sid = null;
		}
	}
}

