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

function _timian_service_price_update($data)
{
    error_log(print_r('weszlo do serwisu',1),0);
    error_log(print_r($data,1),0);
    // getting supplier value
    $uid = $db->select('users', 'u')
    ->fields('u', ['uid'])
    ->condition('name', $data['user_name'])
    ->execute()
    ->fetchAll()[0]->uid;

    $data['sid'] = eldhus_get_supplier_user_supplier_id($uid);


	$insert_values = array();
	// get all entries from the products table having this model number
	$query = db_select('uc_products','p')->fields('p');
	$query->join('field_data_field_commodity_supplier','s','s.entity_id=p.nid');
	$query->join('node','n','n.vid=p.vid');		// to ensure we get the latest revision
	$query->condition('model',$data['model']);
	$query->condition('field_commodity_supplier_value',$data['sid']);
	$rows = $query->execute()->fetchAll();
	$found = count($rows)>0;
	$target_nid = -1;
	foreach ($rows as $item) {
		if ($item->nid !== $target_nid) {
			$context['results']['price_nids'][] = $item->nid; 
			$target_nid = $item->nid;
			$node = node_load($item->nid);
			$prior_discount = isset($node->field_commodity_discount['und'][0]['value'])?$node->field_commodity_discount['und'][0]['value'] / 100:0;
			// NOTE: this check could be an issue with currencies with "higher resolution"
			if (variable_get('timian_upload_always_refresh',true) ||
					(abs($data['list_price'] - $node->list_price) > 0.005) ||
					(abs($data['discount'] - $prior_discount)>0.0005) ||
					(abs($data['contract_price'] - $node->sell_price)>0.005)) {
				
				// ok we always record the weight - change from prior version
				if (!$data['new_weight']) {
					$data['new_weight'] = $node->weight;
				}
				if ($data['discount'] === null) {  // if discount is not set in the input document we use the old/prior value
					$data['discount'] = $node->field_commodity_discount['und'][0]['value'] / 100;		// we multiply back below
				}
				if (!$data['contract_price']) {
					$data['contract_price'] = round((1 - $data['discount']) * $data['list_price']); 
				}
				$insert_values[] = array(
					'nid' => $data['nid'],
					'target_nid' => $target_nid,
					'model' =>  $data['model'],
					'descr' => trim($data['descr']),
					'list_price' =>	$data['list_price'],
					'discount' => $data['discount'],
					'contract_price' =>	$data['contract_price'],
					'date' => $data['date'],
					'weight' => $data['new_weight']
				);
				if (variable_get('timian_auto_price_approve',true)) {
					$insert_values[count($insert_values)-1]['status'] = 1;
					$insert_values[count($insert_values)-1]['prior_list_price'] = $node->list_price;
					$insert_values[count($insert_values)-1]['prior_discount'] = $prior_discount;
					$insert_values[count($insert_values)-1]['prior_contract_price'] = $node->sell_price;
					$node->title = trim($data['descr']);
					$node->list_price = $data['list_price'];
					$node->sell_price = $data['contract_price'];	//TODO: check if there is a store price if we need to recalculate
					$node->field_commodity_discount['und'][0]['value'] = $data['discount']*100;
					if ($data['new_weight']) {
						$insert_values[count($insert_values)-1]['prior_weight'] = $node->weight;
						$node->weight = $data['new_weight']; 
					}
					node_save($node);
				}
			}
		}		
	}
	if (!$found) {
		return [
            "status" => 0
        ];
	}
	else {
		$insert_fields = array('nid','target_nid','model','descr','list_price','discount','contract_price','date','weight');
		if (variable_get('timian_auto_price_approve',true)) {
			$insert_fields[] = 'status';
			$insert_fields[] = 'prior_list_price';
			$insert_fields[] = 'prior_discount';
			$insert_fields[] = 'prior_contract_price';
			if ($data['new_weight']) {
				$insert_fields[] = 'prior_weight';
			}
		}
		$query = db_insert('eldhus_supplier_price_upload')->fields($insert_fields);
		foreach ($insert_values as $values) {
			$query->values($values);
		}
        $query->execute();
        return [
            "status" => 1
        ];
    }
    return [
        "response" => "dupa"
    ];
}
