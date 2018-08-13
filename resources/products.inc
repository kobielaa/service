<?php
//require_once DRUPAL_ROOT . "/" . drupal_get_path('module', 'ubercart') . '/uc_order/uc_order.module';
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
function _timian_service_fetch_data($search)
{

    $query = db_select('node','n')->fields('n',array('nid','title'));
	$query->join('uc_products','p','p.nid=n.nid');
	$query->fields('p',array('model'));
	$query->condition('n.status',true);
    $query->condition('n.type','product');
    $query->join('field_data_field_commodity_supplier','cs','cs.entity_id=n.nid');
    $orCondtion = db_or();
	$orCondtion->condition('n.title','%'.$search.'%','LIKE');
	$orCondtion->condition('p.model',$search.'%','LIKE');
	$query->condition($orCondtion);
    $cursor = $query->execute()->fetchAll();

    $result = [];
    foreach ($cursor as $entry) {
		$match = array();
		$match['id'] = $entry->model;
        $match['text'] = $entry->model.'           | '.trim($entry->title);
        $result[] = $match;
    }
    
       $data = [
           "results" => $result
       ];
    return $data;
}

function _timian_service_fetch_product($model, $title)
{    
    
    $query = db_select('node','n')->fields('n');
	$query->join('uc_products','p','p.nid=n.nid');
	$query->fields('p');
	$query->condition('n.status',true);
    $query->condition('n.type','product');
    $query->join('field_data_field_commodity_supplier','cs','cs.entity_id=n.nid');
    $query->condition('p.model',$model);
    $query->condition('n.title',$title);
    $cursor = $query->execute()->fetchAll();
    $data = [
        'response' => $cursor,

    ];
    
       
    return $data;
}


/**
 * Callback function to delete product from order
 * 
 * @param int $order_id
 * @param int $product_id
 */
function _timian_service_resource_delete( $order_id, $product_id)
{
    uc_order_product_delete($product_id);
    $order = uc_order_load($order_id,true);
    $response = uc_order_save($order);	
    //
    // brakuje dodania komentarza po usunieciu
    $data = [
        [
        'response' => 'success',
        ],
    ];
    
    return $data;
}