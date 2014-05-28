<?php

/**
 * @file
 * Entity hooks and callbacks for donations.
 */

/**
 * Main class for donation entities.
 */
class RedhenDonation extends Entity {

  public $donation_id;
  public $type;
  public $entity_id;
  public $entity_type;
  public $uid = NULL;
  public $order_id = NULL;
  public $amount = NULL;
  public $created;
  public $updated;

  /**
   * Override parent constructor with entity type.
   *
   * @param array $values
   *   Entity values to populate object.
   */
  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_donation');
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $wrapper = entity_metadata_wrapper('redhen_donation', $this);
    return t('Donation for !title', array('!title' => $wrapper->entity->label()));
  }

  /**
   * Add content for RedHen donation custom properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content = parent::buildContent($view_mode, $langcode);
    $wrapper = entity_metadata_wrapper('redhen_donation', $this);
    $author = $wrapper->author->value();
    $transaction_entity_wrapper = $wrapper->transaction_entity;

    $host_entity_type_info = entity_get_info($this->entity_type);
    $host_entity = $wrapper->entity->value();
    list(, , $host_entity_bundle) = entity_extract_ids($this->entity_type, $host_entity);

    $host_label = entity_label($this->entity_type, $host_entity);

    $host_uri = $host_entity ? entity_uri($this->entity_type, $host_entity) : NULL;

    // Link to host entity.
    $host_entity_link_label = (isset($host_entity_type_info['bundles'][$host_entity_bundle]['label'])) ?
      $host_entity_type_info['bundles'][$host_entity_bundle]['label'] : '';

    $content['host_entity_link'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => $host_entity_link_label,
      '#items' => array(
        array(
          '#markup' => l($host_label, $host_uri['path']),
        ),
      ),
      '#weight' => -10,
      '#classes' => 'field field-label-inline clearfix',
    );

    $contact_uri = entity_uri('redhen_contact', $wrapper->contact->value());
    $content['contact'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('RedHen Contact'),
      '#items' => array(
        array(
          '#markup' => l($wrapper->contact->label(), $contact_uri['path']),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $author_uri = entity_uri('user', $author);
    $content['author'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Author'),
      '#items' => array(
        array(
          '#markup' => $author->uid == 0 ? t('Anonymous') : l($author->name, $author_uri['path']),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['pledged'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Pledged'),
      '#items' => array(
        array(
          '#markup' => $wrapper->pledged->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['received'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Received'),
      '#items' => array(
        array(
          '#markup' => $wrapper->received->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['updated'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Updated'),
      '#items' => array(
        array(
          '#markup' => format_date($this->updated),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['status'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Status'),
      '#items' => array(
        array(
          '#markup' => $wrapper->status->value(),
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    $content['transaction'] = array(
      '#type' => 'fieldset',
      '#title' => t('Transaction(s)'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    $rows = array();
    $header = array(t('Order ID'), t('Amount'), t('Order Date'), t('Status'));
    $order_status = commerce_order_statuses();
    switch ($this->transaction_entity_type) {
      case 'commerce_recurring':
        $transaction_entity = $transaction_entity_wrapper->value();
        $transaction_entity_uri = entity_uri($this->transaction_entity_type, $transaction_entity);
        foreach ($transaction_entity_wrapper->commerce_recurring_order->value() as $order) {
          $rows[] = $this->transactionRow($order, $order_status);
        }
        $product_wrapper = $transaction_entity_wrapper->commerce_recurring_ref_product;
        $transactions = array(
          'recurring_donation' => array(
            '#type' => 'fieldset',
            '#title' => l(
              t('Recurring Donation @rec_id',
                array(
                  '@rec_id' => $transaction_entity->id,
                )
              ),
              $transaction_entity_uri['path']
            ),
            'amount' => array(
              '#type' => 'item',
              '#title' => t('Amount'),
              '#markup' => commerce_currency_format(
                $transaction_entity_wrapper->commerce_recurring_fixed_price->amount->value(),
                $transaction_entity_wrapper->commerce_recurring_fixed_price->currency_code->value()
              ),
            ),
            'frequency' => array(
              '#type' => 'item',
              '#title' => t('Frequency'),
              '#markup' => redhen_donation_product_description($product_wrapper),
            ),
            'start_date' => array(
              '#type' => 'item',
              '#title' => t('Start Date'),
              '#markup' => date('m-d-Y H:m',
                $transaction_entity_wrapper->start_date->value()),
            ),
            'end_date' => array(
              '#type' => 'item',
              '#title' => t('End Date'),
              '#markup' => $transaction_entity_wrapper->end_date->value() == 0 ? t('None') : date('m-d-Y H:m', $transaction_entity_wrapper->end_date->value()),
            ),
            'due_date' => array(
              '#type' => 'item',
              '#title' => t('Due Date'),
              '#markup' => date('m-d-Y H:m', $transaction_entity_wrapper->due_date->value()),
            ),
            'status' => array(
              '#type' => 'item',
              '#title' => t('Status'),
              '#markup' => $transaction_entity_wrapper->status->value() ? t('Enabled') : t('Disabled'),
            ),
          ),
          'orders' => array(
            '#theme' => 'table',
            '#header' => $header,
            '#rows' => $rows,
          ),
        );
        break;

      default:
        $transaction_entity = $transaction_entity_wrapper->value();
        $rows[] = $this->transactionRow($transaction_entity, $order_status);
        $transactions = array(
          'var' => array(
            '#markup' => t('One Time Donation'),
          ),
          'stuff' => array(
            '#theme' => 'table',
            '#header' => $header,
            '#rows' => $rows,
          ),
        );
        break;
    }
    $content['transaction']['list'] = $transactions;

    return entity_get_controller($this->entityType)->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Save donation.
   *
   * @see entity_save()
   */
  public function save() {
    $this->updated = REQUEST_TIME;

    if (!$this->donation_id && empty($this->created)) {
      $this->created = REQUEST_TIME;
    }
    return parent::save();
  }

  /**
   * Specify URI.
   */
  protected function defaultUri() {
    return array('path' => 'redhen/donation/' . $this->internalIdentifier());
  }

  /**
   * Helper to build a row in the donation transactions table.
   */
  protected function transactionRow($order, $order_status) {
    $order_uri = entity_uri('commerce_order', $order);

    $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
    $order_total = $order_wrapper->commerce_order_total->value();
    $amount = commerce_currency_format(
      $order_total['amount'],
      $order_total['currency_code']
    );
    return array(
      isset($order_uri['path']) ? l($order->order_id, $order_uri['path']) : $order->order_id,
      $amount,
      date('m-d-Y H:m', $order->changed),
      $order_status[$order->status]['title'],
    );
  }

}
