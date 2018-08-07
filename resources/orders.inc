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
function _timian_service_fetch_data($id)
{
    // $query = db_select('uc_orders','o')->extend('TableSort');

	// $query->join('eldhus_uc_order_fields','cf','o.order_id=cf.order_id');
	// $query->fields('o',array('order_id','created','order_status','order_total','product_count','delivery_first_name','data'));
	// $query->fields('cf',array('delivery_div','delivery_date','order_descr','supplier_id'));
	// $query->join('eldhus_divisions','d','d.id=cf.delivery_div');
	// $query->join('eldhus_business_unit','bu','bu.id=d.bunit');
	// $query->join('uc_order_statuses','os','os.order_status_id=o.order_status');
	// $query->addField('d','code','d_div_code');
	// $query->addField('d','name','d_div_name');
    // $query->addField('bu','name','d_bu_name');
    // $query->condition('o.order_id',12415, "=");
	// $order = $query->execute()->fetchAll();
	
	// $query = db_select('uc_order_products','o');
	// $query->fields('o');
	// $query->condition('o.order_id',12415, "=");
	// $order_products = $query->execute()->fetchAll();

	// $order_data = [
	// 	"order" => $order,
	// 	"products" => $order_products
	// ];

	$order = uc_order_load(12415);
    return [
		"order" => [$order]
	];
}