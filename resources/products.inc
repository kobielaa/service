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
        $match['text'] = $entry->model.'           | ' . $entry->title;
        $result[] = $match;
        error_log(print_r($entry->title,1),0);
    }

       $data = [
           "results" => $result
       ];
    return $data;
}


/**
 * Callback function - get specific product
 * 
 * @param string $model
 * @param string $title
 * 
 * @return array $data Product
 */
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
    $product = $query->execute()->fetchAll();

    $data = [
        'response' => $product,
    ];
    
       
    return $data;
}

function _timian_service_create_product($data) {
    // error_log(print_r($data['title'],1),0);
    $add_product = node_load($data['nid']);
        error_log(print_r($add_product,1),0);

        $quantity = eldhus_parse_number($data['quantity']);
        $weight = eldhus_parse_number($data['weight']);
        $unit_price = eldhus_parse_number($data['price']);
        $discount = eldhus_parse_number($data['discount']);

    	if ($add_product) {
            $add_product_data = array(
                'order_id' => $order->order_id,
                'title' => $form_state['values']['commodity-title'],
                'model' => $form_state['values']['commodity-name'],    // note this is the new model
                'qty' => $add_order_line_qty,
                'weight' => $add_order_line_weight / $add_order_line_qty,
                'price' => $add_order_line_unit_price * (1-$add_order_line_discount/100),
                'cost' => 0,
                'data' => array(
                    'supplier' => array(
                        'total_weight' => $add_order_line_weight,
                        'unit_price' => $add_order_line_unit_price,
                        'qty' => $add_order_line_qty,
                        'discount' => $add_order_line_discount,
                        'model' => $form_state['values']['commodity-name']
                    )
                )
            );
        }
    $response = [
        'quantity' => eldhus_parse_number($data['quantity']),
        'weight' => eldhus_parse_number($data['weight']),
        'unit_price' => eldhus_parse_number($data['price']),
        'discount' => eldhus_parse_number($data['discount'])
    ];

    return $response;
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