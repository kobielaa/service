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
 * Callback function - retrive single order
 *
 * @param int $order_id
 *      Order ID

 * @return array $data 
 *      json with order, order's product, order's comment and statuses
 */
function _timian_service_fetch_order($order_id)
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

	$orderStatus = $order[0]->order_status;
	$order[0]->order_status_icelandic = $statuses['for_suppliers'][$orderStatus];

	unset($statuses['for_suppliers']['received']);
	unset($statuses['for_suppliers']['completed']);
	//Swap english status to islandic


	// add username to comment
	foreach ($comments as $comment) {
		$comment_creator = user_load($comment->uid);
		$comment->user_name = $comment_creator->name;
	}

	$order_data = [
		"order" => $order,
		"products" => $order_products,
		"comments" => $comments,
		"statuses"	=> $statuses
	];

    return $order_data;
}

/**
 * Callback function - update order data
 *
 * @param array $data
 *      Product data
 * @param int $order_id
 *      Product data
 * 
 * @return array $data 
 *      json with order_id
 */
function _timian_service_update_order($order_id, $data){
	$products_ids = [];
	$updated_products = [];

	//retriving products sended from edit order form
	foreach ($data as $key => $value) {
    	if(strpos($key,'weight') !== false){
			$products_ids[] = explode('weight', $key)[1];
		}
	}
	foreach ($data as $key => $value) {
		foreach ($products_ids as $id) {
			if(strpos($key,$id) !== false) {
				$updated_products[$id][explode($id, $key)[0]] = eldhus_parse_number($value);
			}
			
		}
	}

	$order = uc_order_load($order_id, true);
	foreach($updated_products as $key => $value) {
		$product = $order->products[$key];
		$product->price = $value['unit_price'];
		$product->qty = $value['quantity'];
		// $product->weight = $weight / $quantity;
		$product->data['supplier']['total_weight'] = $value['weight'];
		$product->data['supplier']['unit_price'] = $value['unit_price'];
		$product->data['supplier']['qty'] = $value['quantity'];
		$product->data['supplier']['discount'] = $value['discount'];
	}

	$user = user_load_by_name($data['user']);

	$orderStatus = $data['status'];
	$statuses = eldhus_uc_order_status_display(); 
	$data['status'] = array_search($orderStatus,$statuses['for_suppliers']);

	if ($data['status'] != $order->order_status) {	
		$order->order_status = $data['status'];
	}
	if (empty($data['comment'])) {
		uc_order_comment_save($order_id, $user->uid, '-', 'order', $data['status'], false);
	} else {
		uc_order_comment_save($order_id, $user->uid, $data['comment'], 'order',  $data['status'], false);
	}
	

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
	

		uc_order_save($order);

	return [
		"order_id" => $order_id,
	];
}
