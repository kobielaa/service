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

function timian_service_get_single_order($supplier_name, $order_id)
{
    $uc_order = uc_order_load($order_id);

    if ($uc_order) {
        $query = new EntityFieldQuery(); //Check if supplier exists
        $supplier = $query->entityCondition('entity_type', 'node')
            ->propertyCondition('type', 'birgi')
            ->propertyCondition('title', $supplier_name)
            ->range(0, 1)
            ->execute();
        if ($supplier) {
            set_supplier_info_for_order($uc_order); //add supplier info to order 
            if ($uc_order->supplier_name == $supplier_name) { // check if order supplier is same as in request
                $order = new timianOrder($uc_order);
                if (is_array($order) || is_object($order)) {
                    header('Content-Type: application/json');
                    return $order;
                }
            }
        }
        else{
            header("HTTP/1.0 404");
            return ['message' => "There is no such supplier with name = {$supplier_name}", 'status' => 404];
        }
    }   
        header("HTTP/1.0 404");
        return ['message' => "There is no such order with id = {$order_id}", 'status' => 404];
}

function timian_service_get_all_orders($supplier_name, $fromTime, $toTime, $status)
{
    $supplierId = get_node_nid($supplier_name);

    if ($supplierId) {

        $from_timestamp = strtotime($fromTime);
        $to_timestamp = strtotime($toTime);
        $supplier_value = field_get_items('user', user_load_by_name($supplier_name), 'field_user_supplier')[0]['value'];
        $query = db_select('uc_orders', 'o')->fields('o', array('order_id'));

        if (!empty($from_timestamp)) {
            $query->condition('modified', $from_timestamp, '>=');
        }

        if (!empty($to_timestamp)) {
            $query->condition('modified', $to_timestamp, '<=');
        }
        // if procure orders (default to begin with)
        $query->condition('o.data', '%procure%', 'like');

        if (!empty($status) && $status !== 'all') {
            $query->condition('order_status', $status);
        } else {
            $uc_statuses = uc_order_status_list('post_checkout');
            $uc_statuses += uc_order_state_list('completed');
            $uc_statuses = array_map(function ($itm) {return $itm['id'];}, $uc_statuses);
            $query->condition('order_status', $uc_statuses, 'IN');
        }

        $query->join('eldhus_uc_order_fields', 'of', 'of.order_id=o.order_id');
        $query->fields('of', array('supplier_id'));
        $query->leftJoin('node', 'n', 'n.nid=of.supplier_id');
        $query->addField('n', 'title', 'supplier_name');
        $query->leftJoin('field_data_field_supplier_shorthand', 'ss', 'ss.entity_id=of.supplier_id');
        $query->fields('ss', array('field_supplier_shorthand_value'));
        $query->leftJoin('field_data_field_supplier_sid', 'sid', 'sid.entity_id=of.supplier_id');
        $query->fields('sid', array('field_supplier_sid_value'));
        $query->leftJoin('users', 'u', 'u.uid=o.uid');
        $query->fields('u', array('mail'));
        $query->leftJoin('field_data_field_user_gsm', 'ug', 'ug.entity_id=o.uid');
        $query->fields('ug', array('field_user_gsm_value'));
        $query->leftJoin('field_data_field_user_fullname', 'ufn', 'ufn.entity_id=o.uid');
        $query->fields('ufn', array('field_user_fullname_value'));
        $query->condition('of.supplier_id', $supplierId);
        $query->leftJoin('eldhus_divisions', 'd', 'd.id = of.cost_div');
        $query->addField('d', 'code', 'delivery_code');

        $cursor = $query->execute();
        $orders = array();

        foreach ($cursor as $row) {
                  // if ($row->order_id == "4993") { // HARDCODED ESCAPE FOR INVALID LOCAL RECORD
            //     continue;
            // }
            $uc_order = uc_order_load($row->order_id);
            $uc_order->supplier_name = $row->field_supplier_shorthand_value;
            $uc_order->supplier_sid = $row->field_supplier_sid_value;
            $timian_order = new timianOrder($uc_order);
            $timian_order->currency = $uc_order->currency;
            $timian_order->data = $uc_order->data;
            $timian_order->deliveryCode = $row->delivery_code;
            $orders[] = $timian_order;
        }

        return $orders;
    } else {
        header("HTTP/1.0 404");

        return ['message' => "Supplier {$supplier_name} not found", 'status' => 404];
    }
}

function set_supplier_info_for_order(&$order)
{
    if (isset($order->order_id)) {
        $query = db_select("eldhus_uc_order_fields", "o");
        $query->innerJoin('field_data_field_user_supplier', 's', 'o.supplier_id=s.field_user_supplier_value');
        $query->innerJoin('field_data_field_supplier_sid', 'r', 'o.supplier_id=r.entity_id');
        $query->innerJoin('users', 'u', 's.entity_id=u.uid');
        $query->fields("s")->fields("u")->fields("o")->fields("r");
        $query->condition("o.order_id", $order->order_id);

        $result = $query->execute()->fetch();
        $supplier = node_load($result->supplier_id);

        if ($supplier) {
            $order->supplier_name = $supplier->title;
            $order->supplier_sid = $supplier->nid;
        } else {
            $order->supplier_name = t('Supplier not found');
            $order->supplier_sid = null;
        }
    }
}

function get_node_nid($node_title)
{
    $query = db_select('node', 'n')->fields('n')->condition('title', $node_title)->condition('type', 'birgi');
    $node_nid = $query->execute()->fetchAll();
    $node_nid = $node_nid[0]->nid;
    return $node_nid;
}
