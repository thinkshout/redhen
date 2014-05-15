<?php

/**
 * @file
 * Entity hooks and callbacks for donations.
 */

/**
 * Main class for donation entities.
 */
class RedhenDonation extends Entity {

  public
    $donation_id,
    $type,
    $entity_id,
    $entity_type,
    $uid = NULL,
    $order_id = NULL,
    $amount = NULL,
    $created,
    $updated;

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $wrapper = entity_metadata_wrapper('redhen_donation', $this);
    $host = $wrapper->entity->value();
    if ($host) {
      return t('Donation for !title', array(
          '!title' => entity_label($this->entity_type, $host),
        )
      );
    }
    return '';
  }

  /**
   * Build content for Donation.
   *
   * @return array
   *   Render array for a donation entity.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $build = parent::buildContent($view_mode, $langcode);
    $wrapper = entity_metadata_wrapper('redhen_donation', $this);

    $host_entity_type_info = entity_get_info($this->entity_type);
    $host_entity = $wrapper->entity->value();
    list(, , $host_entity_bundle) = entity_extract_ids($this->entity_type, $host_entity);

    $host_label = entity_label($this->entity_type, $host_entity);

    $host_uri = $host_entity ? entity_uri($this->entity_type, $host_entity) : NULL;

    // Link to host entity.
    $host_entity_link_label = (isset($host_entity_type_info['bundles'][$host_entity_bundle]['label'])) ? '<div class="field-label">' . $host_entity_type_info['bundles'][$host_entity_bundle]['label'] . '</div>' : '';
    $build['host_entity_link'] = array(
      '#prefix' => '<div class="field donation-entity-link">' . $host_entity_link_label,
      '#markup' => l($host_label, $host_uri['path']),
      '#suffix' => '</div>',
    );

    $build['created'] = array(
      '#prefix' => '<div class="field donation-created"><div class="field-label">' . t('Created') . '</div>',
      '#markup' => format_date($this->created),
      '#suffix' => '</div>',
    );

    $build['updated'] = array(
      '#prefix' => '<div class="field donation-updated"><div class="field-label">' . t('Updated') . '</div>',
      '#markup' => format_date($this->updated),
      '#suffix' => '</div>',
    );

    return $build;
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

}

