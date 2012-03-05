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
    $label = '',
    $name = '',
    $org_id,
    $membership_id = NULL,
    $org = NULL,
    $author_uid = NULL,
    $start_date = NULL,
    $expire_date = NULL,
    $redhen_state = REDHEN_STATE_ACTIVE,
    $type = '',
    $created = '',
    $updated = '';

  public function __construct(array $values = array()) {
    global $user;

    parent::__construct($values, 'redhen_membership');

    // New membership. is_new might not be set so check for id
    if (!$this->membership_id) {
      $this->author_uid = $user->uid;
      $this->redhen_state = REDHEN_STATE_ACTIVE;
      $this->created = REQUEST_TIME;
    }

    $this->org = $this->organization();
  }

  /**
   * Override buildContent() to add contact properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content['org'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Organization'),
      '#items' => array(
        array(
          '#markup' => check_plain($this->org->label)
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );
    $content['name'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Label'),
      '#items' => array(
        array(
          '#markup' => check_plain($this->label)
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );
    $content['redhen_state'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('State'),
      '#items' => array(
        array(
          '#markup' => ($this->redhen_state == REDHEN_STATE_ACTIVE) ? t('Active') : t('Inactive')
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );

    return entity_get_controller($this->entityType)
      ->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Permanently saves the entity.
   *
   * @return bool
   *  Returns FALSE if entity was not saved.
   */
  public function save() {
    $this->updated = REQUEST_TIME;
    return parent::save();
  }

  /**
   * Set the redhen_state for a membership
   *
   * @param $state
   *  The REDHEN_STATE_* value to use
   */
  public function setState($state) {
    $this->redhen_state = $state;
    $this->save();
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $type = redhen_membership_get_types($this->type);
    return $type->label . ': ' . $this->label;
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'redhen/org/' . $this->org_id . '/memberships/' . $this->identifier());
  }

  /**
   * Return an organization object if possible.
   *
   * @return RedhenOrg|null
   */
  protected function organization() {
    $org = NULL;
    if (isset($this->org_id)) {
      $org = redhen_org_load($this->org_id);
    }
    return $org;
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
      'filter' => TRUE,
    );
    $properties['expire_date'] = array(
      'label' => t("Expiration Date"),
      'description' => t("The date the membership expires."),
      'type' => 'date',
      'field_type' => 'textfield',
      'schema field' => 'expire_date',
      'filter' => TRUE,
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
    $properties['redhen_state'] = array(
      'label' => t('State'),
      'filter' => TRUE,
      'field_type' => 'select',
      'options' => array(
        REDHEN_STATE_ACTIVE => t('Active'),
        REDHEN_STATE_ARCHIVED => t('Archived')
      ),
    ) + $properties['redhen_state'];

    return $info;
  }
}

