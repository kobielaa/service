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
 * Callback function - autocomplete products
 *
 * @param string $search
 *  Searched product
 * @return array $data
 *   Products match to serach term
 */
function _timian_service_fetch_data($search)
{
    $query = db_select('node', 'n')->fields('n', ['nid', 'title']);
    $query->join('uc_products', 'p', 'p.nid=n.nid');
    $query->fields('p', ['model']);
    $query->condition('n.status', true);
    $query->condition('n.type', 'product');
    $query->join('field_data_field_commodity_supplier', 'cs', 'cs.entity_id=n.nid');
    $orCondtion = db_or();
    $orCondtion->condition('n.title', '%' . $search . '%', 'LIKE');
    $orCondtion->condition('p.model', $search . '%', 'LIKE');
    $query->condition($orCondtion);
    $cursor = $query->execute()->fetchAll();

    $result = [];
    foreach ($cursor as $entry) {
        $match = [];
        $match['id'] = $entry->model;
        $match['text'] = $entry->model . '           | ' . $entry->title;
        $result[] = $match;
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
    $query = db_select('node', 'n')->fields('n');
    $query->join('uc_products', 'p', 'p.nid=n.nid');
    $query->fields('p');
    $query->condition('n.status', true);
    $query->condition('n.type', 'product');
    $query->join('field_data_field_commodity_supplier', 'cs', 'cs.entity_id=n.nid');
    $query->condition('p.model', $model);
    $query->condition('n.title', $title);
    $product = $query->execute()->fetchAll();

    $data = [
        'response' => $product,
    ];

    return $data;
}


/**
 * Callback function - update product from order
 *
 * @param array $data
 *      Product data
 * @return array $data
 *      json with order_id
 */
function _timian_service_resource_edit($data){
    $order = uc_order_load($data['order']);
    // $order['products']
    // uc_order_save($order);

    // $quantity = eldhus_parse_number($data['quantity']);
    // $weight = eldhus_parse_number($data['weight']);
    // $unit_price = eldhus_parse_number($data['price']);
    // $discount = eldhus_parse_number($data['discount']);

    // $add_product = node_load($data['nid']);
    // $user = user_load_by_name($data['supplier']);
    error_log(print_r('gowno z dupy',1),0);
    error_log(print_r($order->products[data['product_id']],1),0);
    $dane = [
        "response" => $data['order'],
    ];

    return $dane;
}

/**
 * Callback function - add new product to order
 *
 * @param array $data
 *      Product data
 * @return array $data
 *      json with order_id
 */
function _timian_service_create_product($data)
{
    $add_product = node_load($data['nid']);
    $user = user_load_by_name($data['supplier']);

    $quantity = eldhus_parse_number($data['quantity']);
    $weight = eldhus_parse_number($data['weight']);
    $unit_price = eldhus_parse_number($data['price']);
    $discount = eldhus_parse_number($data['discount']);

    if ($add_product) {
        $add_product_data = [
            'order_id' => $data['order_id'],
            'title'    => $data['description'],
            'model'    => $data['model'],
            'qty'      => $quantity,
            'weight'   => $weight,
            'price'    => $unit_price * (1 - $discount / 100),
            'cost'     => 0,
            'data'     => [
                'supplier' => [
                    'total_weight' => $weight,
                    'unit_price'   => $unit_price,
                    'qty'          => $quantity,
                    'discount'     => $discount,
                    'model'        => $data['model']
                ]
            ]
        ];

        // if (arg(0) == 'supplier' && arg(1) == 'view' && arg(2) == 'order') {
        //     uc_order_comment_save($order->order_id, $user->uid, "Bætti við vörulínu: ".$add_product->model." - ".$add_product->title,"order",$order->order_status,false);
        // }
        $add_product_data['nid'] = $add_product->nid;
        $add_product_data['weight_units'] = $add_product->weight_units;
        $add_order_product = entity_create('uc_order_product', $add_product_data);
        uc_order_product_entity_save($add_order_product);
        $order = uc_order_load($data['order_id'], true);
        uc_order_comment_save($order->order_id, $user->uid, "Bætti við vörulínu: " . $add_product->model . " - " . $add_product->title, "order", $order->order_status, false);
        uc_order_save($order);

    }
    $response = [
        'order_id' => $data['order_id'],
    ];

    return $response;
}


/**
 * Callback function to delete product from order
 *
 * @param array $data
 *      Product's data
 *
 * @return array $response
 *      json with order_id and product name
 *
 */
function _timian_service_resource_delete($data)
{
    $user = user_load_by_name($data['supplier']);
    $order = uc_order_load($data['order_id'], true);

    uc_order_product_delete($data['product_id']);
    uc_order_save($order);
    uc_order_comment_save(
        $data['order_id'],
        $user->uid,
        "Eyddi út vörulínu: " . $data['model'] . " - " . $data['title'] . ", magn: " . $data['weight'] * $data['quantity'] . " " . $data['weight'] . ", " . theme('uc_price', ['price' => $data['price'] * $data['quantity']]), "order", $order->order_status, false);

    $response = [
        [
            'order_id' => $data['order_id'],
            'product'  => $data['title'],
        ],
    ];

    return $response;
}