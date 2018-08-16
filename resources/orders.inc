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
    $query->condition('o.order_id', $order_id , "=");
	$order = $query->execute()->fetchAll();
	

	$order_products = eldhus_uc_get_order_products_for_display(uc_order_load($order_id,true));
	$comments = uc_order_comments_load($order_id);
	$statuses = eldhus_uc_order_status_display();

	$order_data = [
		"order" => $order,
		"products" => $order_products,
		"comments" => $comments,
		"statuses"	=> $statuses
	];

    return $order_data;
}