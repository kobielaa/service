<?php

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
function _timian_service_fetch_data($order_id)
{
    $query = db_select('uc_orders','o')->extend('TableSort');

	$query->join('eldhus_uc_order_fields','cf','o.order_id=cf.order_id');
	$query->fields('o',array('order_id','created','order_status','order_total','product_count','delivery_first_name','data'));
	$query->fields('cf',array('delivery_div','delivery_date','order_descr','supplier_id'));
	$query->join('eldhus_divisions','d','d.id=cf.delivery_div');
	$query->join('eldhus_business_unit','bu','bu.id=d.bunit');
	$query->join('uc_order_statuses','os','os.order_status_id=o.order_status');
	$query->addField('d','code','d_div_code');
	$query->addField('d','name','d_div_name');
	$query->addField('bu','name','d_bu_name');
	$query->addField('cf','ext_invoice_id');
	$query->addField('cf','ext_invoice_date');
    $query->condition('o.order_id', $order_id , "=");
	$order = $query->execute()->fetchAll();
	

	$order_products = eldhus_uc_get_order_products_for_display(uc_order_load($order_id,true));
	$comments = uc_order_comments_load($order_id);
	$statuses = eldhus_uc_order_status_display();

	// add username to comment
	foreach ($comments as $comment) {
		$comment_creator = user_load($comment->uid);
		$comment['user_name'] = $comment_creator->name;
	}

	$order_data = [
		"order" => $order,
		"products" => $order_products,
		"comments" => $comments,
		"statuses"	=> $statuses
	];

    return $order_data;
}

function _timian_service_update_order($order_id, $data){
	
	$order = uc_order_load($order_id, true);
	$user = user_load_by_name($data['supplier']);

	error_log(print_r($data['status'],1),0);
	error_log(print_r($order->order_status,1),0);
	if ($data['status'] != $order->order_status) {	
		uc_order_update_status($order_id, $data['status']);
	}
	if (empty($data['comment'])) {
		uc_order_comment_save($order_id, $user->uid, '-', 'order', $data['status'], false);
	} else {
		uc_order_comment_save($order_id, $user->uid, $data['comment'], 'order',  $data['status'], false);
	}
	
	$order = uc_order_load($order_id, true);
	$changed = false;
	if (isset($data['ext_invoice_id'])) {
		$invoice_ids = explode(',',$data['ext_invoice_id']);
		if (count($invoice_ids) > 1) {
			// incase we have more than one we must trim out all extra spaces
			$invoice_ids = array_map(function ($itm) {return trim($itm);},$invoice_ids);
			$order->ext_invoice_id = implode(',',$invoice_ids);
		}
		else {
			$order->ext_invoice_id = $data['ext_invoice_id'];
		}
		$changed = true;
	}
	if (isset($data['ext_invoice_date'])) {
		$order->ext_invoice_date = strtotime($data['ext_invoice_date']);
		$changed = true;
	}
	
	if ($changed) {
		
		uc_order_save($order);
	}

	
	return [
		"order_id" => $order_id,
	];
}
