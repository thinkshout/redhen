<?php
/**
 * @file
 * Redhen Group entity classes
 */

/**
 * The class used for group entities.
 */
class RedhenMembership extends Entity {

  public
    $note = '',
    $name = '',
    $entity_type,
    $entity_id,
    $entity = NULL,
    $membership_id = NULL,
    $author_uid = NULL,
    $start_date = NULL,
    $expire_date = NULL,
    $membership_state = REDHEN_MEMBERSHIP_ACTIVE,
    $type = '',
    $created = '',
    $updated = '';

  public function __construct(array $values = array()) {
    global $user;

    parent::__construct($values, 'redhen_membership');

    // New membership. is_new might not be set so check for id
    if (!$this->membership_id) {
      $this->author_uid = $user->uid;
      $this->membership_state = REDHEN_MEMBERSHIP_ACTIVE;
      $this->created = REQUEST_TIME;
    }

    $this->entity = $this->entity();
  }

  /**
   * Override buildContent() to add contact properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $entity_uri = entity_uri($this->entity->entityType(), $this->entity);
    $content['entity'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Applies to'),
      '#items' => array(
        array(
          '#markup' => l($this->entity->label(), $entity_uri['path'])
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );
    if ($this->note != '') {
      $content['note'] = array(
        '#theme' => 'redhen_property_field',
        '#label' => t('Note'),
        '#items' => array(
          array(
            '#markup' => check_plain($this->note)
          ),
        ),
        '#classes' => 'field field-label-inline clearfix',
      );
    }

    if ($this->start_date) {
      $content['start_date'] = array(
        '#theme' => 'redhen_property_field',
        '#label' => t('Start Date'),
        '#items' => array(
          array(
            '#markup' => date('m/d/Y', strtotime($this->start_date))
          ),
        ),
        '#classes' => 'field field-label-inline clearfix',
      );
    }
    if ($this->expire_date) {
      $content['expire_date'] = array(
        '#theme' => 'redhen_property_field',
        '#label' => t('Expiration Date'),
        '#items' => array(
          array(
            '#markup' => date('m/d/Y', strtotime($this->expire_date))
          ),
        ),
        '#classes' => 'field field-label-inline clearfix',
      );
    }

    $membership_states = redhen_membership_state_options();
    $content['membership_state'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('State'),
      '#items' => array(
        array(
          '#markup' => $membership_states[$this->membership_state],
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Set the redhen_state for a membership
   *
   * @param $state
   *  The REDHEN_STATE_* value to use
   */
  public function setState($state) {
    $this->membership_state = $state;
    $this->save();
  }

  /**
   * Override label() to add membership label.
   */
  public function label() {
    $type = redhen_membership_get_types($this->type);
    // $this->entity may not be populated yet
    $entity = reset(entity_load($this->entity_type, array($this->entity_id)));
    return "{$type->label} membership for {$entity->label()}";
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'redhen/membership/' . $this->identifier());
  }

  /**
   * Return an entity object if possible.
   *
   * @return Entity|null
   */
  protected function entity() {
    $entity = NULL;
    if (isset($this->entity_type) && isset($this->entity_id)) {
      $entity = reset(entity_load($this->entity_type, array($this->entity_id)));
    }
    return $entity;
  }

}

/**
 * Controls metadata for Redhen memberships
 *
 * Redhen utilizes custom property attributes to determine if a property
 * should be available as a filter on the listing page. These attributes are
 *   filter: TRUE to add as a filter.
 *   field_type: textfield, select, etc.
 *   options: array of options for select field types.

 */
class RedhenMembershipMetadataController extends EntityDefaultMetadataController {
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['start_date'] = array(
      'label' => t("Start Date"),
      'description' => t("The date the membership begins."),
      'type' => 'date',
      'field_type' => 'textfield',
      'schema field' => 'start_date',
      'filter' => FALSE,
    );
    $properties['expire_date'] = array(
      'label' => t("Expiration Date"),
      'description' => t("The date the membership expires."),
      'type' => 'date',
      'field_type' => 'textfield',
      'schema field' => 'expire_date',
      'filter' => FALSE,
    );
    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the membership was created."),
      'type' => 'date',
      'schema field' => 'created',
    ) + $properties['created'];
    $properties['updated'] = array(
      'label' => t("Updated"),
      'description' => t("The date the membership was updated."),
      'type' => 'date',
      'schema field' => 'updated',
    ) + $properties['updated'];
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the membership record."),
      'schema field' => 'author_uid'
    );
    $properties['membership_state'] = array_merge($properties['membership_state'], array(
      'label' => t('State'),
      'filter' => TRUE,
      'field_type' => 'list',
      'schema field' => 'membership_state',
      'options list' => 'redhen_membership_state_options',
    ));
    return $info;
  }
}

