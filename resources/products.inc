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
        "results" => $result,
    ];
    return $data;
}

// /**
//  * Callback function - get specific product
//  *
//  * @param string $supplier
//  *
//  * @return array $data Product
//  */
function _timian_service_fetch_products($data)
{
    $supplierId = get_node_nid($data['supplier']);

    if ($supplierId) {
        $query = db_select('node', 'n')->fields('n');
        $query->fields('n', array('nid', 'title', 'changed'));
        $query->leftJoin('field_data_body', 'nb', 'nb.revision_id=n.vid');
        $query->join('uc_products', 'p', 'p.vid=n.vid');
        $query->fields('p', array('model', 'list_price', 'sell_price', 'weight', 'weight_units', 'is_offer'));
        $query->join('field_data_field_commodity_supplier', 'sidx', 'sidx.entity_id=n.nid');
        $query->leftJoin('field_data_field_commodity_discount', 'd', 'd.entity_id=n.nid');
        $query->fields('d', array('field_commodity_discount_value'));
        $query->leftJoin('field_data_field_commodity_units_sale', 'u_sale', 'u_sale.entity_id=n.nid');
        $query->fields('u_sale', array('field_commodity_units_sale_value'));
        $query->leftJoin('field_data_field_commodity_saleunit', 's_unit', 's_unit.entity_id=n.nid');
        $query->fields('s_unit', array('field_commodity_saleunit_value'));
        $query->leftJoin('field_data_field_commodity_unspsc', 'pidx', 'pidx.entity_id=n.nid');
        $query->fields('pidx', array('field_commodity_unspsc_code'));
        $query->leftJoin('eldhus_unspsc', 'us', 'us.code=pidx.field_commodity_unspsc_code');
        $query->fields('us', array('descr'));
        $query->fields('nb', array('body_value'));
        $query->condition('sidx.field_commodity_supplier_value', $supplierId);
        $query->condition('n.status', 1);

        if ($data['is_offer'] == "true") {
            $query->condition('p.is_offer', 0, '>');
        }

        $header=[];
        if (isset($data['category'])) {
            
            switch ($data['category']) {
                // case 'everything':
                //     $query->orderBy('p.title', 'DESC');          
                //     break;
                case 'model':
                    $query->condition('p.model', $data['term'] . '%', 'LIKE');
                    $query->orderBy('p.model', 'ASC');    
                    break;
                case 'title':
                    $query->condition('n.title', '%' . $data['term'] . '%', 'LIKE');
                    $query->orderBy('n.title', 'ASC'); 
                    break;
                case 'description':
                    $query->condition('nb.body_value', '%' . $data['term'] . '%', 'LIKE');
                    $query->orderBy('nb.body_value', 'ASC'); 
                    break;
                case 'unspsc';
                    $query->condition('us.descr', '%' . $data['term'] . '%', 'LIKE');
                    $query->orderBy('us.descr', 'ASC');
                    break;
                default:
                    $query->orderBy('n.title', 'ASC'); 
                    break;
            }
        } 
        if (isset($data['purchasers_numbers'])) { //TODO check if number      

            $max_pages = 300 / intval($data['purchasers_numbers']);

            if (isset($data['page'])) {
                $query->range((intval($data['page'])-1) * $max_pages, $max_pages);
            }
            else{
                $query->range(0, $max_pages);
            }
        }
        //$db_or = db_or();
        // if (isset($data['model'])) {
        //     $db_or->condition('p.model', $data['model'] . '%', 'LIKE');
        // }
        // if (isset($data['title'])) {
        //     $db_or->condition('n.title', '%' . $data['title'] . '%', 'LIKE');
        // }
        // if (isset($data['body_value'])) {
        //     $db_or->condition('nb.body_value', '%' . $data['body_value'] . '%', 'LIKE');
        // }
        // if (isset($data['unspsc_description'])) {
        //     $db_or->condition('us.descr', '%' .  $data['unspsc_description'] . '%', 'LIKE');
        // }

        //$query->condition($db_or);
        $products = $query->execute()->fetchAll(); //defalut drupal is sorting by nodeif if no order by is provided
        $products['products_count'] = count($products);
        

        return $products ? $products : "None products found for supplier {$data['supplier']}";
    } else {
        header("HTTP/1.0 404");

        return ['message' => 'Supplier not found', 'status' => 404];
    }
}

/**
 * Callback function - update product from order
 *
 * @param array $data
 *      Product data
 * @return array $data
 *      json with order_id
 */

// [872] => stdClass Object
// (
//     [order_product_id] => 872
//     [order_id] => 169
//     [nid] => 3574
//     [title] => ( Bruður Holland Toast)
//     [model] => N710100
//     [qty] => 2.000
//     [cost] => 0.00000
//     [price] => 500
//     [weight] => 0.25
//     [weight_units] => kg
//     [data] => Array
//         (
//             [module] => uc_product
//             [supplier] => Array
//                 (
//                     [model] => N710100
//                     [total_weight] => 0.5
//                     [unit_price] => 217
//                     [qty] => 2
//                     [discount] => 0
//                     [org_price] => 217.00000
//                     [org_qty] => 2.000
//                     [org_weight] => 0.2
//                     [surplus] => 0.1
//                 )

//         )

function _timian_service_resource_edit($data)
{
    $quantity = eldhus_parse_number($data['quantity']);
    $weight = eldhus_parse_number($data['weight']);
    $unit_price = eldhus_parse_number($data['unit_price']);
    $discount = eldhus_parse_number($data['discount']);

    $order = uc_order_load($data['order']);
    $product = $order->products[$data['product_id']];
    $product->price = $unit_price;
    $product->qty = $quantity;
    // $product->weight = $weight / $quantity;
    $product->data['supplier']['total_weight'] = $weight;
    $product->data['supplier']['unit_price'] = $unit_price;
    $product->data['supplier']['qty'] = $quantity;
    $product->data['supplier']['discount'] = $discount;

    uc_order_save($order);

    $response = [
        "order_id" => $data['order'],
    ];

    return $response;
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
            'title' => $data['description'],
            'model' => $data['model'],
            'qty' => $quantity,
            'weight' => $weight,
            'price' => $unit_price * (1 - $discount / 100),
            'cost' => 0,
            'data' => [
                'supplier' => [
                    'total_weight' => $weight,
                    'unit_price' => $unit_price,
                    'qty' => $quantity,
                    'discount' => $discount,
                    'model' => $data['model'],
                ],
            ],
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
            'product' => $data['title'],
        ],
    ];

    return $response;
}

function get_node_nid($node_title)
{
    $query = db_select('node', 'n')->fields('n')->condition('title', $node_title)->condition('type', 'birgi');
    $node_nid = $query->execute()->fetchAll();
    $node_nid = $node_nid[0]->nid;
    return $node_nid;
}
